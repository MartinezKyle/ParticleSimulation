import java.io.*;
import java.net.*;
import java.awt.Point;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.concurrent.*;
import java.util.stream.Collectors;
import java.util.zip.GZIPOutputStream;
import java.util.HashMap;
import javax.swing.SwingUtilities;
import java.nio.charset.StandardCharsets;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class ParticleSimulationServer {
    private static ParticleSimulation particleSimulation;
    private ServerSocket serverSocket;
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
                ClientHandler handler = new ClientHandler(clientSocket, this, clientSocket.getPort());
                clientHandlers.add(handler); 
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

    public void broadcastExplorer(Explorer explorer, int client_id) {
        clientHandlers.forEach(handler -> {
            if (handler.returnID() != client_id) {
                try {
                    handler.sendExplorer(explorer);
                } catch (IOException e) {
                    System.err.println("Error broadcasting state: " + e.getMessage());
                }
            }
        });
    }

    public class ClientHandler implements Runnable {
        private final Socket clientSocket;
        private final int clientID;
        private ParticleSimulationServer server;
        protected DataOutputStream dos;
        protected DataInputStream dis;

        public ClientHandler(Socket socket, ParticleSimulationServer server, int clientID) throws IOException {
            this.clientSocket = socket;
            this.server = server;
            this.clientID = clientID;

            dos = new DataOutputStream(clientSocket.getOutputStream());
            dis = new DataInputStream(clientSocket.getInputStream());

            sendID();
        }

        @Override
        public void run() {
            try {
                while (true) { 
                    String command = dis.readUTF(); 
        
                    System.out.println("Command: " + command);
                    String[] parts = command.split(" ");
                    for (String part: parts){
                        System.out.println(part);
                    }

                    if ("ExplorerCoordinates".equals(parts[0])){
                        double x = Double.parseDouble(parts[1]);
                        double y = Double.parseDouble(parts[2]);
                        int index = particleSimulation.simulationPanel.explorerExist(clientID);
                        System.out.println("Index: " + index);
                        if (index != -1){
                            particleSimulation.simulationPanel.updateExplorer(clientID, x, y);
                        }
                        else {
                            particleSimulation.simulationPanel.addExplorer(clientID,x, y);
                            System.out.println("ClientID: " + clientID);
                        }

                        server.broadcastExplorer(new Explorer(clientID, x, y), clientID);
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
                        .map(e -> new ExplorerState(e.getClientID(), e.getXCoord(), e.getYCoord()))
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
            ExplorerState state = new ExplorerState(e.getClientID(), e.getXCoord(), e.getYCoord());
        
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

        public void sendID() throws IOException {
            String typeID = "ID";
            
            ObjectMapper mapper = new ObjectMapper();
            HashMap<String, Integer> clientIDMap = new HashMap<>();
            clientIDMap.put("clientID", clientID);
            String json = mapper.writeValueAsString(clientIDMap);
            
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try (GZIPOutputStream gzipOut = new GZIPOutputStream(baos)) {
                gzipOut.write(json.getBytes(StandardCharsets.UTF_8));
            } 
            
            sendTypedMessage(typeID, baos.toByteArray());
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

        public int returnID(){
            return this.clientID;
        }
              
    }

    public static void displayGUI(boolean isDevMode) {
        SwingUtilities.invokeLater(() -> particleSimulation.setVisible(true));
    }

    public static void main(String[] args) {
        String configFile = "config.json";
        ObjectMapper mapper = new ObjectMapper();
        try {
            JsonNode config = mapper.readTree(new File(configFile));
            int port = config.get("port").asInt(); 
            
            ParticleSimulationServer server = new ParticleSimulationServer(port);
            new Thread(server::start).start();
            
            particleSimulation = new ParticleSimulation(true);
            particleSimulation.simulationPanel.setServer(server);
            displayGUI(true);
        } catch (IOException e) {
            System.err.println("Error reading config or starting server: " + e.getMessage());
        }
    }
}
