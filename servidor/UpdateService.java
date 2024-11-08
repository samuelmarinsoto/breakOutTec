import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.HashMap;

/**
 * UpdateService manages client connections and handles updates to and from connected clients.
 * It maintains a map of active client channels, and provides methods for accepting new connections,
 * processing updates from clients, and pushing updates to all connected clients.
 */
public class UpdateService {

    private final HashMap<SocketChannel, ClientHandler> clients; // Tracks connected clients and their handlers.

    /**
     * Initializes the UpdateService with an empty map of clients.
     */
    public UpdateService() {
        clients = new HashMap<>();
    }

    /**
     * Accepts a new client connection on a server socket and registers it for non-blocking I/O.
     *
     * @param socket The SelectionKey associated with the server socket.
     * @param selector The selector that will monitor events for this channel.
     */
    public void acceptConnection(SelectionKey socket, Selector selector){
        ServerSocketChannel serverChannel = (ServerSocketChannel) socket.channel();
        try {
        	SocketChannel clientChannel = serverChannel.accept();
        	clientChannel.configureBlocking(false);
        	clientChannel.register(selector, SelectionKey.OP_READ);

        	ClientHandler client = new ClientHandler(clientChannel);
        	clients.put(clientChannel, client);
        	System.out.println("New client connected: " + clientChannel.getRemoteAddress());
        } catch (IOException e){
        	System.err.println("Failed to accept new connection in acceptConnection() in UpdateService.java");
        }
    }

    /**
     * Processes incoming data from a client and pushes updates to all clients if a valid message is received.
     *
     * @param socket The SelectionKey associated with the client socket.
     */
    public void processUpdate(SelectionKey socket){
        SocketChannel clientChannel = (SocketChannel) socket.channel();
        ClientHandler client = clients.get(clientChannel);

		try {
        	if (client != null) {
            	String message = client.read();
	            if (message == null) {
         	       clientChannel.close();
         	       clients.remove(clientChannel);
         	       System.out.println("Client disconnected.");
            	} else {
                	System.out.println("Received from client: " + message);
                	pushUpdate(message);
            	}
            }
        } catch (IOException e){
        	System.err.println("Error processing update in processUpdate() in UpdateService.java");
        }
    }

    /**
     * Pushes a specified update message to all connected clients. Removes a client if it fails to receive the update.
     *
     * @param update The update message to be sent to all clients.
     */
    public void pushUpdate(String update) {
        for (ClientHandler client : clients.values()){
        	int result = client.write(update);
        	if (result == -1){
        		SocketChannel clientChannel = client.get();
        		try {
        			clientChannel.close();
        		} catch (IOException e){
        			System.err.println("Failed to close client channel in pushUpdate() in UpdateService.java");
        		}
        		clients.remove(clientChannel);
        		System.out.println("Removed client due to write failure");
        	}
        }
    }
}
