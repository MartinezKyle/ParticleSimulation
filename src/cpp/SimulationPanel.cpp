#include "SimulationPanel.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <boost/thread.hpp>
#include <mutex>
#include <memory>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

#include "Particle.hpp"
#include "Explorer.hpp"

using json = nlohmann::json;

SimulationPanel::SimulationPanel(bool isDev) : isDevMode(isDev) {
    setServer(nullptr);
    frameCount = 0;
    previousFPS = 0;
    lastFPSCheck = std::chrono::high_resolution_clock::now();
    particles.reserve(1000);

    if (!font.loadFromFile("../../lib/calibri.ttf")) {
        std::cerr << "Failed to load font file" << std::endl;
    }
}

void SimulationPanel::setServer(void* server) {
    // Not implemented for this example
}

void SimulationPanel::parseJSONToParticles(const json& jsonData) {
    std::cout << "Called 2 " << std::endl;

    if (jsonData.is_array()) {
        for (const auto& obj : jsonData) {
            // Extract values from each JSON object in the array
            double angle = obj.at("angle").get<double>();
            double velocity = obj.at("velocity").get<double>();
            double xcoord = obj.at("xcoord").get<double>();
            double ycoord = obj.at("ycoord").get<double>();

            // Correctly construct a shared_ptr<Particle> and add it to the vector
            particles.push_back(std::make_shared<Particle>(angle, velocity, xcoord, ycoord));
        }
    } else {
        double angle = jsonData["angle"];
        double velocity = jsonData["velocity"];
        double xcoord = jsonData["xcoord"];
        double ycoord = jsonData["ycoord"];

        // Construct a shared_ptr<Particle> and add it to the vector
        particles.push_back(std::make_shared<Particle>(angle, velocity, xcoord, ycoord));
    }

    std::cout << "particle size: " << particles.size() << std::endl;
}

void SimulationPanel::addExplorer(double x, double y) {
    explorer = std::make_shared<Explorer>(x, y);
}

const std::shared_ptr<Explorer>& SimulationPanel::getExplorer() const {
    return explorer;
}

void SimulationPanel::updateSimulation() {
    for (auto& particle : particles) {
        particle->updatePosition(0.00169);
    }

    frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFPSCheck).count();
    if (timeDiff >= 500) {
        frameCount /= (timeDiff / 1000.0);
        previousFPS = frameCount;
        frameCount = 0;
        lastFPSCheck = currentTime;
    }
}

void SimulationPanel::changeDevMode(bool isDev) {
    isDevMode = isDev;
}

void SimulationPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.clear(sf::Color::White);
    for (const auto& particle : particles) {
        target.draw(*particle);
    }

     if (explorer) {
        target.draw(*explorer);
    }
    
    drawFPSInfo(target);   
}


void SimulationPanel::drawFPSInfo(sf::RenderTarget& target) const {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::Green);
    text.setPosition(10, 20);

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFPSCheck).count();

    if (timeDiff >= 500) {
        text.setString("FPS: " + std::to_string(previousFPS));
    } else {
        text.setString("FPS: Calculating...");
    }

    target.draw(text);
}

