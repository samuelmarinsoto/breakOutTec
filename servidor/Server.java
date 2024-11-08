import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.Scanner;
import org.json.JSONObject;

/**
 * Server class that handles incoming client connections and manages the game state through the console.
 * Uses a non-blocking I/O model with a Selector to accept and process client updates.
 */
public class Server {

    private Selector selector = null; // Selector for managing non-blocking channels.
    private final UpdateService updateService; // Service to handle client updates.

    /**
     * Initializes the Server on a specified port, setting up non-blocking I/O and starting the console thread.
     *
     * @param port The port number for the server to listen on.
     */
    public Server(int port){
    	try {
        	// Setup selector and server socket channel
        	selector = Selector.open();
        	ServerSocketChannel serverChannel = ServerSocketChannel.open();
        	serverChannel.configureBlocking(false);
        	serverChannel.bind(new InetSocketAddress(port));
        	serverChannel.register(selector, SelectionKey.OP_ACCEPT);
		} catch (IOException e){
			System.err.println("Socket Channel or selector setup failed");
			System.exit(1);
		}
        // Initialize UpdateService
        updateService = new UpdateService();

        System.out.println("Server started. Listening on port " + port);

        // Start console input thread for manual game updates
        new Thread(this::console).start();
    }

    /**
     * Console input handler that reads user inputs for game updates and sends these updates to all clients.
     */
    private void console(){
    	Scanner scanner = new Scanner(System.in);
    	while (true){
    		System.out.println("Enter game update:\n"
    		+ "(format: brick_index action extra_life increase_ball_speed decrease_ball_speed"
    		+ " double_racket half_racket add_ball color score player_position ball_position_x ball_position_y)\n"
    		+ "(example: 3 create true false true false true false red 1500 50 50 50)\n");
    		String input = scanner.nextLine();
    		JSONObject json = parsecmd(input);

    		if (json != null){
    			updateService.pushUpdate(json.toString());
    			System.out.println("sent: " + json.toString());
    		} else {
    			System.out.println("Invalid input format. Please try again");
    		}
    	}
    }

    /**
     * Parses a command-line input string into a JSON object for structured game updates.
     *
     * @param input The raw input string from the console.
     * @return JSONObject representing the game update, or null if input format is invalid.
     */
    private JSONObject parsecmd(String input){
    	String[] parts = input.split(" ");
    	if (parts.length != 13) return null; // Validate argument count

    	JSONObject json = new JSONObject();

        json.put("brick_index", Integer.parseInt(parts[0]));
        json.put("action", parts[1]); // "create" or "destroy"
        json.put("extra_life", Boolean.parseBoolean(parts[2]));
        json.put("increase_ball_speed", Boolean.parseBoolean(parts[3]));
        json.put("decrease_ball_speed", Boolean.parseBoolean(parts[4]));
        json.put("double_racket", Boolean.parseBoolean(parts[5]));
        json.put("half_racket", Boolean.parseBoolean(parts[6]));
        json.put("add_ball", Boolean.parseBoolean(parts[7]));
        json.put("color", parts[8]);
        json.put("score", Integer.parseInt(parts[9]));
        json.put("player_position", Integer.parseInt(parts[10]));
        json.put("ball_position_x", Integer.parseInt(parts[11]));
        json.put("ball_position_y", Integer.parseInt(parts[12]));

    	return json;
    }

    /**
     * Starts the server's main loop, accepting and processing client connections.
     */
    public void start(){
        while (true) {
        	try {
            	selector.select(); // Block until events are available
            } catch (IOException e){
            	System.err.println("Selector error");
            	System.exit(1);
            }
            Iterator<SelectionKey> activeSockets = selector.selectedKeys().iterator();

            while (activeSockets.hasNext()) {
                SelectionKey socket = activeSockets.next();
                activeSockets.remove();

                if (socket.isAcceptable()) {
                    updateService.acceptConnection(socket, selector); // Delegate new connections to UpdateService
                } else if (socket.isReadable()) {
                    updateService.processUpdate(socket); // Delegate client data handling to UpdateService
                }
            }
        }
    }

    /**
     * Main entry point for the server application.
     *
     * @param args Command-line arguments.
     */
    public static void main(String[] args){
    	Server server = new Server(12345);
    	server.start();
    }
}
