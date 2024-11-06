import java.io.*;
import java.net.*;

public class SpectatorClient {

    private String serverAddress;
    private int port;

    // Constructor that takes server address and port
    public SpectatorClient(String serverAddress, int port) {
        this.serverAddress = serverAddress;
        this.port = port;
    }

    // Method to connect to the server and receive messages
    public void start() {
        try (Socket socket = new Socket(serverAddress, port);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            System.out.println("Connected to server as a spectator.");

            // Listen and print server messages
            String serverMessage;
            while ((serverMessage = in.readLine()) != null) {
                System.out.println("Server: " + serverMessage);
            }

        } catch (IOException e) {
            System.out.println("Error connecting to server");
            e.printStackTrace();
        }
    }

    // Main method to run the spectator client
    public static void main(String[] args) {
        String serverAddress = "127.0.0.1"; // Localhost
        int port = 8080; // Server port

        SpectatorClient client = new SpectatorClient(serverAddress, port);
        client.start();
    }
}
