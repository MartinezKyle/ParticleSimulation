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
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    simulationPanel.addExplorer(mousePos.x, mousePos.y);
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

//getSimulationPanel method implementation
SimulationPanel& ParticleSimulation::getSimulationPanel(){
    return simulationPanel;
}


