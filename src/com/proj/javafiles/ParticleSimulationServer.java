import java.io.*;
import java.net.*;
import java.awt.Point;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.concurrent.*;
import java.util.stream.Collectors;
import java.util.zip.GZIPOutputStream;
import javax.swing.SwingUtilities;
import java.nio.charset.StandardCharsets;
import com.fasterxml.jackson.databind.ObjectMapper;

public class ParticleSimulationServer {
    private static ParticleSimulation particleSimulation;
    private ServerSocket serverSocket;
    private final ConcurrentHashMap<Integer, Point> explorerPositions = new ConcurrentHashMap<>();
    private final ExecutorService clientExecutor = Executors.newCachedThreadPool();
    // Added: A list to keep track of all client handlers for broadcasting updates
    public final List<ClientHandler> clientHandlers = new CopyOnWriteArrayList<>();

    public ParticleSimulationServer(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        System.out.println("Server started on port: " + port);
    }

    public void start() {
        try {
            while (!serverSocket.isClosed()) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress() + ":" + clientSocket.getPort());
                ClientHandler handler = new ClientHandler(clientSocket, explorerPositions, this);
                clientHandlers.add(handler); // Keep track of all connected clients
                clientExecutor.submit(handler);
                broadcastSimulationState();
            }
        } catch (IOException e) {
            System.out.println("Server exception: " + e.getMessage());
        } finally {
            stop();
        }
    }

    public void stop() {
        try {
            clientExecutor.shutdown();
            serverSocket.close();
        } catch (IOException e) {
            System.out.println("Error closing server: " + e.getMessage());
        }
    }

    // Added: A method to broadcast the simulation state to all connected clients
    public void broadcastSimulationState() {
        clientHandlers.forEach(handler -> {
            try {
                handler.sendState();
            } catch (IOException e) {
                System.err.println("Error broadcasting state: " + e.getMessage());
            }
        });
    }

    public void broadcastParticle(Particle p) {
        clientHandlers.forEach(handler -> {
            try {
                handler.sendParticle(p);
            } catch (IOException e) {
                System.err.println("Error broadcasting state: " + e.getMessage());
            }
        });
    }

    public void broadcastExplorer(Explorer explorer) {
        clientHandlers.forEach(handler -> {
            try {
                handler.sendExplorer(explorer);
            } catch (IOException e) {
                System.err.println("Error broadcasting state: " + e.getMessage());
            }
        });
    }

    public class ClientHandler implements Runnable {
        private final Socket clientSocket;
        private final ConcurrentHashMap<Integer, Point> explorerPositions;
        private ParticleSimulationServer server;
        protected DataOutputStream dos;
        protected DataInputStream dis;

        public ClientHandler(Socket socket, ConcurrentHashMap<Integer, Point> positions, ParticleSimulationServer server) {
            this.clientSocket = socket;
            this.explorerPositions = positions;
            this.server = server; 
        }

        @Override
        public void run() {
            try {
                dos = new DataOutputStream(clientSocket.getOutputStream());
                dis = new DataInputStream(clientSocket.getInputStream());

                while (true) { // Keep listening for commands
                    String command = dis.readUTF(); // This will block until a command is received
        
                    System.out.println("Command: " + command);
                    String[] parts = command.split(" ");
                    for (String part: parts){
                        System.out.println(part);
                    }
                    if ("connect".equals(command)) {
                        dos.writeUTF("connected");
                        sendState(); // Send initial state upon connection
                    } else if ("disconnect".equals(command)) { // Example disconnect command
                        break; // Exit the loop if "disconnect" command is received
                    } else if ("ExplorerCoordinates".equals(parts[0])){
                        double x = Double.parseDouble(parts[1]);
                        double y = Double.parseDouble(parts[2]);
                        int index = particleSimulation.simulationPanel.explorerExist(0);
                        if (index != -1){
                            particleSimulation.simulationPanel.updateExplorer(index, x, y);
                            System.out.println("Updating");
                        }
                        else {
                            particleSimulation.simulationPanel.addExplorer(0,x, y);
                        }
                    }
                    // Handle other commands as needed
                }
            } catch (IOException e) {
                System.out.println("Client handler exception: " + e.getMessage());
            } finally {
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    System.out.println("Error closing client socket: " + e.getMessage());
                }
                // Remove this handler from the server's list upon disconnection
                server.clientHandlers.remove(this);
                System.out.println("Client disconnected and handler removed.");
            }
        } 

        public byte[] serializeSimulationState(String type) throws IOException {
            // SimulationState state = null;
            Object state = null;
        
            if ("Particles".equals(type)) {
                if (particleSimulation.simulationPanel.particles.isEmpty()) {
                    // System.out.println("Particle is empty.");
                    return new byte[0];
                }
        
                List<ParticleState> particleStates = particleSimulation.simulationPanel.particles.stream()
                        .map(p -> new ParticleState(p.getXCoord(), p.getYCoord(), p.getVelocity(), p.getAngle()))
                        .collect(Collectors.toList());
        
                state = new SimulationState(particleStates).getParticles();
        
            } else if ("Explorers".equals(type)) {
                if (particleSimulation.simulationPanel.explorers.isEmpty()) {
                    // System.out.println("Explorer is empty.");
                    return new byte[0];
                }
        
                List<ExplorerState> explorerStates = particleSimulation.simulationPanel.explorers.stream()
                        .map(e -> new ExplorerState(e.getXCoord(), e.getYCoord()))
                        .collect(Collectors.toList());
        
                state = new SimulationState(explorerStates, true).getExplorers();;
            }
        
            if (state == null) {
                return new byte[0];
            }
        
            ObjectMapper mapper = new ObjectMapper();
            String json = mapper.writeValueAsString(state);
        
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try (GZIPOutputStream gzipOut = new GZIPOutputStream(baos)) {
                gzipOut.write(json.getBytes(StandardCharsets.UTF_8));
            }
            return baos.toByteArray();
        }        

        public byte[] serializeParticle(Particle p) throws IOException {
            ParticleState state = new ParticleState(p.getXCoord(), p.getYCoord(), p.getVelocity(), p.getAngle());
        
            ObjectMapper mapper = new ObjectMapper();
            String json = mapper.writeValueAsString(state);
        
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try (GZIPOutputStream gzipOut = new GZIPOutputStream(baos)) {
                gzipOut.write(json.getBytes(StandardCharsets.UTF_8));
            }
            return baos.toByteArray();
        }   

        public byte[] serializeExplorer(Explorer e) throws IOException {
            ExplorerState state = new ExplorerState(e.getXCoord(), e.getYCoord());
        
            ObjectMapper mapper = new ObjectMapper();
            String json = mapper.writeValueAsString(state);
        
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try (GZIPOutputStream gzipOut = new GZIPOutputStream(baos)) {
                gzipOut.write(json.getBytes(StandardCharsets.UTF_8));
            }
            return baos.toByteArray();
        }   

        public void sendState() throws IOException {
            // Example type indicators
            String typeParticle = "Particles";
            String typeExplorer = "Explorers";
        
            // Serialize your state here (this is just an example)
            byte[] serializedParticleState = serializeSimulationState(typeParticle);
            byte[] serializedExplorerState = serializeSimulationState(typeExplorer);
        
            if (serializedParticleState.length > 0) {
                sendTypedMessage(typeParticle, serializedParticleState);
            }
            
            if (serializedExplorerState.length > 0) {
                sendTypedMessage(typeExplorer, serializedExplorerState);
            }
        }

        public void sendParticle(Particle p) throws IOException {
            // Example type indicators
            String typeParticle = "Particles";
        
            // Serialize your state here (this is just an example)
            byte[] serializedParticleState = serializeParticle(p);
        
            if (serializedParticleState.length > 0) {
                sendTypedMessage(typeParticle, serializedParticleState);
            }
        }

        public void sendExplorer(Explorer e) throws IOException {
            String typeExplorer = "Explorers";
        
            // Serialize your state here (this is just an example)
            byte[] serializedExplorerState = serializeExplorer(e);
            
            if (serializedExplorerState.length > 0) {
                sendTypedMessage(typeExplorer, serializedExplorerState);
            }
        }
        
        private void sendTypedMessage(String type, byte[] data) throws IOException {
            dos.flush();
            
            dos.writeUTF(type);
            dos.flush();
        
            ByteBuffer buffer = ByteBuffer.allocate(4);
            buffer.putInt(data.length);
            dos.write(buffer.array());
            dos.flush();
        
            dos.write(data);
            dos.flush();
        }
              
    }

    public static void displayGUI(boolean isDevMode) {
        SwingUtilities.invokeLater(() -> particleSimulation.setVisible(true));
    }

    public static void main(String[] args) {
        int port = 1234;
        try {
            ParticleSimulationServer server = new ParticleSimulationServer(port);
            new Thread(server::start).start();
            // Assuming ParticleSimulation constructor does not require server as parameter
            particleSimulation = new ParticleSimulation(true);
            particleSimulation.simulationPanel.setServer(server);
            displayGUI(true);
        } catch (IOException e) {
            System.err.println("Server failed to start: " + e.getMessage());
        }
    }
}
