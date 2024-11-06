import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.HashMap;

public class UpdateService {

    private final HashMap<SocketChannel, ClientHandler> clients;

    public UpdateService() {
        clients = new HashMap<>();
    }

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
        	System.err.println("clientchannel failed in acceptConnection() in UpdateService.java");
        }
    }

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
        	System.err.println("clientchannel failed in processUpdate() in UpdateService.java");
        }
    }

    public void pushUpdate(String update) {
        for (ClientHandler client : clients.values()){
        	int result = client.write(update);
        	if (result == -1){
        		SocketChannel clientChannel = client.get();
        		try {
        			clientChannel.close();
        		} catch (IOException e){
        			System.err.println("clientchannel failed in acceptConnection() in UpdateService.java");
        		}
        		clients.remove(clientChannel);
        		System.out.println("Removed client due to write failure");
        	}
        }
    }
}
