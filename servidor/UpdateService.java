import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.HashMap;

public class UpdateService {

    private final HashMap<SocketChannel, ClientHandler> clients;

    public UpdateService() {
        clients = new HashMap<>();
    }

    public void acceptConnection(SelectionKey key, Selector selector) throws IOException {
        ServerSocketChannel serverChannel = (ServerSocketChannel) key.channel();
        SocketChannel clientChannel = serverChannel.accept();
        clientChannel.configureBlocking(false);
        clientChannel.register(selector, SelectionKey.OP_READ);

        ClientHandler client = new ClientHandler(clientChannel);
        clients.put(clientChannel, client);
        System.out.println("New client connected: " + clientChannel.getRemoteAddress());
    }

    public void processUpdate(SelectionKey key) throws IOException {
        SocketChannel clientChannel = (SocketChannel) key.channel();
        ClientHandler client = clients.get(clientChannel);

        if (clientHandler != null) {
            String message = clientHandler.read();
            if (message == null) {
                clientChannel.close();
                clients.remove(clientChannel);
                System.out.println("Client disconnected.");
            } else {
                System.out.println("Received from client: " + message);
                clientHandler.write("Echo: " + message);
            }
        }
    }

    public void updateAllClients(String update) {
        clients.values().forEach(client -> client.write(update));
    }
}
