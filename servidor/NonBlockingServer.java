import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Iterator;

public class NonBlockingServer {

    private static final int PORT = 12345;

    private static final Selector selector

	public static void start(){
		try {
			 // 1. Open a ServerSocketChannel in non-blocking mode
            ServerSocketChannel serverChannel = ServerSocketChannel.open();
            serverChannel.configureBlocking(false);
            serverChannel.bind(new InetSocketAddress(PORT));

            // 2. Open a Selector for managing multiple channels
            Selector selector = Selector.open();
            
            // Register the serverChannel with the selector for "ACCEPT" events
            serverChannel.register(selector, SelectionKey.OP_ACCEPT);

            System.out.println("Server started. Listening on port " + PORT);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
    public static void main(String[] args) {
        try {

            while (true) {
                // Wait for an event (blocking call with no timeout)
                selector.select();
                
                // Get all ready events
                Iterator<SelectionKey> keyIterator = selector.selectedKeys().iterator();
                while (keyIterator.hasNext()) {
                    SelectionKey key = keyIterator.next();
                    keyIterator.remove();

                    if (key.isAcceptable()) {
                        // 4. Accept new client connections
                        ServerSocketChannel server = (ServerSocketChannel) key.channel();
                        SocketChannel clientChannel = server.accept();
                        clientChannel.configureBlocking(false);
                        System.out.println("Connected to client: " + clientChannel.getRemoteAddress());

                        // Register the new client with the selector for read events
                        clientChannel.register(selector, SelectionKey.OP_READ);
                    } else if (key.isReadable()) {
                        // 5. Handle incoming data from a client
                        SocketChannel clientChannel = (SocketChannel) key.channel();
                        ByteBuffer buffer = ByteBuffer.allocate(256);
                        
                        // Read data into buffer
                        int bytesRead = clientChannel.read(buffer);
                        if (bytesRead == -1) {
                            // Client has closed the connection
                            clientChannel.close();
                            System.out.println("Client disconnected.");
                            continue;
                        }

                        // Print the received message
                        buffer.flip();
                        byte[] bytes = new byte[buffer.remaining()];
                        buffer.get(bytes);
                        String message = new String(bytes);
                        System.out.println("Received from client: " + message);

                        // 6. Echo the message back to the client
                        buffer.clear();
                        buffer.put(("Echo: " + message).getBytes());
                        buffer.flip();
                        clientChannel.write(buffer);
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
