// Include the corresponding header file
#include "ParticleSimulation.hpp"

// Constructor implementation
ParticleSimulation::ParticleSimulation() : simulationPanel(true) {
    simulationPanel.setPosition(50, 50);
}

// run method implementation
void ParticleSimulation::run() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Simulation Panel");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        simulationPanel.updateSimulation();
        window.clear();
        window.draw(simulationPanel);
        window.display();
    }
}

// addParticle method implementation
void ParticleSimulation::addParticle(const json& jsonData){
    std::cout << "Called 1 " << std::endl;
    simulationPanel.parseJSONToParticles(jsonData);
}
