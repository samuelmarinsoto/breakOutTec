import java.io.*;
import java.net.*;

public class SingletonSocketServer {

    private static SingletonSocketServer instance;
    private ServerSocket serverSocket;
    private boolean isRunning = false;
    private final int port = 8080; // specify the port to listen on

    // Private constructor to prevent instantiation
    private SingletonSocketServer() {
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            System.out.println("Could not start server on port: " + port);
            e.printStackTrace();
        }
    }

    // Public method to get the singleton instance
    public static SingletonSocketServer getInstance() {
        if (instance == null) {
            synchronized (SingletonSocketServer.class) {
                if (instance == null) {
                    instance = new SingletonSocketServer();
                }
            }
        }
        return instance;
    }

    // Method to start the server
    public void startServer() {
        isRunning = true;
        System.out.println("Server started on port: " + port);
        
        while (isRunning) {
            try {
                // Listen for a connection
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress());
                
                // Handle client connection in a separate thread
                new ClientHandler(clientSocket).start();
            } catch (IOException e) {
                System.out.println("Error accepting connection");
                e.printStackTrace();
            }
        }
    }

    // Method to stop the server
    public void stopServer() {
        isRunning = false;
        try {
            if (serverSocket != null) {
                serverSocket.close();
            }
        } catch (IOException e) {
            System.out.println("Error closing the server");
            e.printStackTrace();
        }
    }

    // Client handler to process each client in a separate thread
    private static class ClientHandler extends Thread {
        private Socket clientSocket;

        public ClientHandler(Socket socket) {
            this.clientSocket = socket;
        }

        @Override
        public void run() {
            try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                 PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true)) {
                 
                String inputLine;
                while ((inputLine = in.readLine()) != null) {
                    System.out.println("Received: " + inputLine);
                    out.println("Echo: " + inputLine); // Simple echo server
                }
            } catch (IOException e) {
                System.out.println("Error handling client connection");
                e.printStackTrace();
            } finally {
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    // Main method to run the server
    public static void main(String[] args) {
        SingletonSocketServer server = SingletonSocketServer.getInstance();
        server.startServer();
    }
}