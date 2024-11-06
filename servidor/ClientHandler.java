import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.List;
import org.json.JSONObject;

public class ClientHandler {
	private final SocketChannel clientChannel; // channel es el tunel que abre el socket (no se)

	public ClientHandler(SocketChannel clientChannel){
		this.clientChannel = clientChannel;
	}

	public SocketChannel get(){
		return this.clientChannel;
	}

	public int write(String update){
		ByteBuffer buffer = ByteBuffer.wrap(update.getBytes());

		try {
			int byteswritten = this.clientChannel.write(buffer);
			if (byteswritten == -1){
				System.err.println("Write failed, client closed connection");
				return -1;
			}
			if (byteswritten == 0){
				System.err.println("Write failed, update too big for buffer");
				return 0;
			}
		} catch (IOException e) {
			System.err.println("Write failed, client disconnected?: " + e.getMessage());
			return -1;
		}

		return 0;
	}

	public String read(){
		ByteBuffer buffer = ByteBuffer.allocate(1024);
		try {
			int bytesread = this.clientChannel.read(buffer);
			if (bytesread == -1){
				System.err.println("Read failed, client closed connection");
				return null;
			}
		} catch (IOException e) {
			System.err.println("Read failed, client disconnected abruptly: " + e.getMessage());
			return null;
		}
		
		buffer.flip(); // network order is big endian, we read little endian
		String jsonmessage = new String(buffer.array(), 0, buffer.limit());
		System.out.println(jsonmessage); // debug
		
		return jsonmessage;	
	}
	
}
