import java.io.*;
import java.net.*;

public class Client {

    private String serverAddress;
    private int port;

    // Constructor that takes server address and port
    public Client(String serverAddress, int port) {
        this.serverAddress = serverAddress;
        this.port = port;
    }

    // Method to connect to the server and send/receive messages
    public void start() {
        try (Socket socket = new Socket(serverAddress, port);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))) {

            System.out.println("Connected to server at " + serverAddress + ":" + port);

            String userInput;
            System.out.print("Enter message (type 'exit' to quit): ");
            while ((userInput = stdIn.readLine()) != null) {
                if (userInput.equalsIgnoreCase("exit")) {
                    break;
                }

                // Send message to the server
                out.println(userInput);

                // Read and print the response from the server
                String serverResponse = in.readLine();
                System.out.println("Server response: " + serverResponse);

                System.out.print("Enter message (type 'exit' to quit): ");
            }

        } catch (IOException e) {
            System.out.println("Error connecting to server");
            e.printStackTrace();
        }
    }

    // Main method to run the client
    public static void main(String[] args) {
        String serverAddress = "127.0.0.1"; // Localhost
        int port = 8080; // Server port

        Client client = new Client(serverAddress, port);
        client.start();
    }
}
