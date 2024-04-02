#include "ParticleSimulation.hpp"

ParticleSimulation::ParticleSimulation() : simulationPanel() {
    simulationPanel.setPosition(50, 50);
}

void ParticleSimulation::updateSimulationLoop() {
    while (isRunning) {
        simulationPanel.updateSimulation();
        
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
}

void ParticleSimulation::run() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Simulation Panel");
    
    while (window.isOpen() && isRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;
            }
            else if (event.type == sf::Event::MouseButtonPressed && simulationPanel.getExplorer() == nullptr) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    simulationPanel.addExplorer(this->ID, mousePos.x, mousePos.y);
                    std::cout << "Added explorer at: " << mousePos.x << ", " << mousePos.y << std::endl;
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                std::shared_ptr<Explorer> explorer = simulationPanel.getExplorer();
                if (explorer){
                    if (event.key.code == sf::Keyboard::W) {
                        std::cout << "W pressed" << std::endl;
                        explorer->moveUp();
                    }
                    else if (event.key.code == sf::Keyboard::S) {
                        std::cout << "S pressed" << std::endl;
                        explorer->moveDown();
                    }
                    else if (event.key.code == sf::Keyboard::A) {
                        std::cout << "A pressed" << std::endl;
                        explorer->moveLeft();
                    }
                    else if (event.key.code == sf::Keyboard::D) {
                        std::cout << "D pressed" << std::endl;
                        explorer->moveRight();
                    }
                }
            }
        }

        window.clear();
        window.draw(simulationPanel);
        window.display();
    }
}

void ParticleSimulation::setID(const json& jsonData) { 
    if (!jsonData.is_object()) {
        std::cerr << "Expected jsonData to be an object, got: " << jsonData.type_name() << std::endl;
        return;
    }

    if (jsonData.contains("clientID") && jsonData["clientID"].is_number()) {
        this->ID = jsonData["clientID"];
    } else {
        std::cerr << "Missing or invalid 'clientID' in jsonData." << std::endl;
    }
}

void ParticleSimulation::addParticle(const json& jsonData){
    simulationPanel.parseJSONToParticles(jsonData);
}

void ParticleSimulation::addOtherExplorer(const json& jsonData){
    simulationPanel.parseJSONToExplorers(jsonData, "add");
}

void ParticleSimulation::removeExplorer(const json& jsonData){
    simulationPanel.parseJSONToExplorers(jsonData, "remove");
}

bool ParticleSimulation::getIsRunning() const {
    return isRunning.load();
}

SimulationPanel& ParticleSimulation::getSimulationPanel(){
    return simulationPanel;
}


