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

	public void write(String update){
		ByteBuffer buffer = ByteBuffer.wrap(update.getBytes());
		this.clientChannel.write(buffer);
	}

	public String read(){
		ByteBuffer buffer = ByteBuffer.allocate(1024);

		// TODO: handle client disconnect ( read() returns -1)
		this.clientChannel.read(buffer);
		buffer.flip(); // network order is big endian, we read little endian
		String jsonmessage = new String(buffer.array(), 0, buffer.limit());
		System.out.println(jsonmessage); // debug
		
		return jsonmessage;
	}
	
}
