import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.HashMap;

public class UpdateService {

    private final HashMap<SocketChannel, ClientHandler> clients;

    public UpdateService() {
        clients = new HashMap<>();
    }

    public void acceptConnection(SelectionKey socket, Selector selector) throws IOException {
        ServerSocketChannel serverChannel = (ServerSocketChannel) socket.channel();
        SocketChannel clientChannel = serverChannel.accept();
        clientChannel.configureBlocking(false);
        clientChannel.register(selector, SelectionKey.OP_READ);

        ClientHandler client = new ClientHandler(clientChannel);
        clients.put(clientChannel, client);
        System.out.println("New client connected: " + clientChannel.getRemoteAddress());
    }

    public void processUpdate(SelectionKey socket) throws IOException {
        SocketChannel clientChannel = (SocketChannel) socket.channel();
        ClientHandler client = clients.get(clientChannel);

        if (client != null) {
            String message = client.read();
            if (message == null) {
                clientChannel.close();
                clients.remove(clientChannel);
                System.out.println("Client disconnected.");
            } else {
                System.out.println("Received from client: " + message);
                client.write("Echo: " + message);
            }
        }
    }

    public void pushUpdate(String update) {
        clients.values().forEach(client -> client.write(update));
    }
}
