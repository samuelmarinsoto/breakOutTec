import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.*;
import java.util.Iterator;

public class Server {

    private final Selector selector;
    private final UpdateService updateService;

    public Server(int port) throws IOException {
        // Setup selector and server socket channel
        selector = Selector.open();
        ServerSocketChannel serverChannel = ServerSocketChannel.open();
        serverChannel.configureBlocking(false);
        serverChannel.bind(new InetSocketAddress(port));
        serverChannel.register(selector, SelectionKey.OP_ACCEPT);

        // Initialize UpdateService
        updateService = new UpdateService();

        System.out.println("Server started. Listening on port " + port);
    }

    public void start() throws IOException {
        while (true) {
            selector.select(); // Block until events are available
            Iterator<SelectionKey> activeSockets = selector.selectedKeys().iterator();

            while (activeSockets.hasNext()) {
                SelectionKey socket = activeSockets.next();
                activeSockets.remove();

                if (socket.isAcceptable()) {
                    updateService.acceptConnection(socket, selector); // Delegate to UpdateService
                } else if (socket.isReadable()) {
                    updateService.processUpdate(socket); // Delegate to UpdateService
                }
            }
        }
    }

    public static void main(String[] args){
    	Server server = new Server(12345);
    	server.start();
    }
}
