import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import org.json.JSONObject;

/**
 * The {@code ClientHandler} class is responsible for managing a single client connection
 * using a non-blocking {@link SocketChannel}. It provides methods to read from and
 * write to the client channel.
 */
public class ClientHandler {
    private final SocketChannel clientChannel;

    /**
     * Constructs a new {@code ClientHandler} instance with a specified client {@link SocketChannel}.
     *
     * @param clientChannel the client channel to be handled by this instance
     */
    public ClientHandler(SocketChannel clientChannel) {
        this.clientChannel = clientChannel;
    }

    /**
     * Returns the client {@link SocketChannel} managed by this handler.
     *
     * @return the client {@code SocketChannel}
     */
    public SocketChannel get() {
        return this.clientChannel;
    }

    /**
     * Sends an update message to the client by writing a string to the client channel.
     *
     * @param update the message to be sent to the client as a string
     * @return {@code -1} if the client closed the connection, {@code 0} if the buffer is
     *         too small to write the entire update, or a positive value indicating success
     */
    public int write(String update) {
        ByteBuffer buffer = ByteBuffer.wrap(update.getBytes());

        try {
            int byteswritten = this.clientChannel.write(buffer);
            if (byteswritten == -1) {
                System.err.println("Write failed, client closed connection");
                return -1;
            }
            if (byteswritten == 0) {
                System.err.println("Write failed, update too big for buffer");
                return 0;
            }
        } catch (IOException e) {
            System.err.println("Write failed, client disconnected?: " + e.getMessage());
            return -1;
        }

        return 0;
    }

    /**
     * Reads a message from the client. This method blocks until data is available or the client
     * closes the connection. It reads the data into a buffer and converts it to a JSON string.
     *
     * @return the message received from the client as a string, or {@code null} if the client
     *         closed the connection or an error occurred during reading
     */
    public String read() {
        ByteBuffer buffer = ByteBuffer.allocate(1024);
        try {
            int bytesread = this.clientChannel.read(buffer);
            if (bytesread == -1) {
                System.err.println("Read failed, client closed connection");
                return null;
            }
        } catch (IOException e) {
            System.err.println("Read failed, client disconnected abruptly: " + e.getMessage());
            return null;
        }

        buffer.flip(); // Prepares buffer for reading
        String jsonmessage = new String(buffer.array(), 0, buffer.limit());
        System.out.println(jsonmessage); // Debugging output
        
        return jsonmessage;
    }
}
