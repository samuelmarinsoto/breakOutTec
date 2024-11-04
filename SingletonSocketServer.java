import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class SingletonSocketServer {

    private static SingletonSocketServer instance;
    private ServerSocket serverSocket;
    private boolean isRunning = false;
    private final int port = 8080;
    private volatile ClientHandler mainClientHandler;
    private List<SpectatorHandler> spectators = new ArrayList<>();

    private SingletonSocketServer() {
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            System.out.println("Could not start server on port: " + port);
            e.printStackTrace();
        }
    }

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

    public void startServer() {
        isRunning = true;
        System.out.println("Server started on port: " + port);

        // Start a thread to read console input
        new Thread(this::handleConsoleInput).start();

        while (isRunning) {
            try {
                Socket clientSocket = serverSocket.accept();
                System.out.println("\nClient connected: " + clientSocket.getInetAddress().getHostAddress());

                if (mainClientHandler == null) {
                    mainClientHandler = new ClientHandler(clientSocket, this);
                    mainClientHandler.start();
                    System.out.println("Main client connected.");
                } else {
                    SpectatorHandler spectatorHandler = new SpectatorHandler(clientSocket, this);
                    spectators.add(spectatorHandler);
                    spectatorHandler.start();
                    System.out.println("Spectator connected.");
                }

            } catch (IOException e) {
                System.out.println("\nError accepting connection");
                e.printStackTrace();
            }
        }
    }

    private void handleConsoleInput() {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.println("\n--- Server Console ---");
            System.out.println("1. Send greeting");
            System.out.println("2. Send alert");
            System.out.println("3. Send goodbye");
            System.out.println("4. Exit");
            System.out.print("Choose an option: ");

            String choice = scanner.nextLine();
            String message = null;

            switch (choice) {
                case "1":
                    message = "GREETING: Hello, Main Client!";
                    break;
                case "2":
                    message = "ALERT: This is an alert!";
                    break;
                case "3":
                    message = "GOODBYE: Goodbye, Main Client!";
                    break;
                case "4":
                    stopServer();
                    return;
                default:
                    System.out.println("Invalid option. Try again.");
                    continue;
            }

            if (mainClientHandler != null && message != null) {
                mainClientHandler.sendMessage(message);
                System.out.println("Message sent to main client: " + message);
            } else {
                System.out.println("No main client connected.");
            }
        }
    }

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

    private static class ClientHandler extends Thread {
        private Socket clientSocket;
        private PrintWriter out;
        private SingletonSocketServer server;

        public ClientHandler(Socket socket, SingletonSocketServer server) {
            this.clientSocket = socket;
            this.server = server;
        }

        @Override
        public void run() {
            try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))) {
                out = new PrintWriter(clientSocket.getOutputStream(), true);
                out.println("Welcome to the server!");

                String inputLine;
                while ((inputLine = in.readLine()) != null) {
                    System.out.println("Main client sent: " + inputLine);
                    server.broadcastToSpectators("Main client says: " + inputLine);
                }
            } catch (IOException e) {
                System.out.println("Error handling main client connection");
                e.printStackTrace();
            } finally {
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public void sendMessage(String message) {
            if (out != null) {
                out.println(message);
            }
        }
    }

    private static class SpectatorHandler extends Thread {
        private Socket spectatorSocket;
        private PrintWriter out;

        public SpectatorHandler(Socket socket, SingletonSocketServer server) {
            this.spectatorSocket = socket;
        }

        @Override
        public void run() {
            try {
                out = new PrintWriter(spectatorSocket.getOutputStream(), true);
                out.println("You are now connected as a spectator.");
            } catch (IOException e) {
                System.out.println("Error handling spectator connection");
                e.printStackTrace();
            }
        }

        public void sendMessageToSpectator(String message) {
            if (out != null) {
                out.println(message);
            }
        }
    }

    public void broadcastToSpectators(String message) {
        for (SpectatorHandler spectator : spectators) {
            spectator.sendMessageToSpectator(message);
        }
    }

    public static void main(String[] args) {
        SingletonSocketServer server = SingletonSocketServer.getInstance();
        server.startServer();
    }
}
