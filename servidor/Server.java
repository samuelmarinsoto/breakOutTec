import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.Scanner;
import org.json.JSONObject;

public class Server {

    private Selector selector = null;
    private final UpdateService updateService;

    public Server(int port){
    	try {
        	// Setup selector and server socket channel
        	selector = Selector.open();
        	ServerSocketChannel serverChannel = ServerSocketChannel.open();
        	serverChannel.configureBlocking(false);
        	serverChannel.bind(new InetSocketAddress(port));
        	serverChannel.register(selector, SelectionKey.OP_ACCEPT);
		} catch (IOException e){
			System.err.println("Socket Channel or selector failed");
			System.exit(1);
		}
        // Initialize UpdateService
        updateService = new UpdateService();

        System.out.println("Server started. Listening on port " + port);

        // start console
        new Thread(this::console).start();
    }

    private void console(){
    	Scanner scanner = new Scanner(System.in);
    	while (true){
    		System.out.println("Enter game update:\n"
    		+ "(format: brick_index extra_life increase_ball_speed decrease_ball_speed"
    		+ " double_racket half_racket add_ball color score)\n"
    		+ "(example: 3 true false true false true false red 1500)\n");
    		String input = scanner.nextLine();
    		JSONObject json = parsecmd(input);

    		if (json != null){
    			updateService.pushUpdate(json.toString());
    		} else {
    			System.out.println("Invalid input format. Please try again");
    		}
    	}
    }

    private JSONObject parsecmd(String input){
    	String[] parts = input.split(" ");
    	if (parts.length != 9) return null; // not enough arguments

    	JSONObject json = new JSONObject();
    	json.put("brick_index", Integer.parseInt(parts[0]));
    	json.put("extra_life", Boolean.parseBoolean(parts[1]));
    	json.put("increase_ball_speed", Boolean.parseBoolean(parts[2]));
    	json.put("decrease_ball_speed", Boolean.parseBoolean(parts[3]));
    	json.put("double_racket", Boolean.parseBoolean(parts[4]));
    	json.put("half_racket", Boolean.parseBoolean(parts[5]));
    	json.put("add_ball", Boolean.parseBoolean(parts[6]));
    	json.put("color", parts[7]);
    	json.put("score", Integer.parseInt(parts[8]));

    	return json;
    }

    public void start(){
        while (true) {
        	try {
            	selector.select(); // Block until events are available
            } catch (IOException e){
            	System.err.println("Peto selector");
            	System.exit(1);
            }
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
