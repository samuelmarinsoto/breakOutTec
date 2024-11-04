import java.io.*;
import java.net.*;

public class MainClient {

    private String serverAddress;
    private int port;

    public MainClient(String serverAddress, int port) {
        this.serverAddress = serverAddress;
        this.port = port;
    }

    public void start() {
        try (Socket socket = new Socket(serverAddress, port);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))) {

            System.out.println("Connected to server at " + serverAddress + ":" + port);

            new Thread(() -> {
                try {
                    String serverMessage;
                    while ((serverMessage = in.readLine()) != null) {
                        handleServerMessage(serverMessage);
                    }
                } catch (IOException e) {
                    System.out.println("Error reading server messages");
                    e.printStackTrace();
                }
            }).start();

            String userInput;
            while ((userInput = stdIn.readLine()) != null) {
                if (userInput.equalsIgnoreCase("exit")) {
                    break;
                }
                out.println(userInput);
            }

        } catch (IOException e) {
            System.out.println("Error connecting to server");
            e.printStackTrace();
        }
    }

    private void handleServerMessage(String message) {
        if (message.startsWith("GREETING")) {
            System.out.println("Received greeting from server.");
        } else if (message.startsWith("ALERT")) {
            System.out.println("Received alert from server!");
        } else if (message.startsWith("GOODBYE")) {
            System.out.println("Received goodbye from server. Closing connection...");
            System.exit(0);
        } else {
            System.out.println("Server: " + message);
        }
    }

    public static void main(String[] args) {
        String serverAddress = "127.0.0.1"; // Localhost
        int port = 8080; // Server port

        MainClient client = new MainClient(serverAddress, port);
        client.start();
    }
}
