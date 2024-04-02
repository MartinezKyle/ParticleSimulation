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
    explorer = nullptr;
    frameCount = 0;
    previousFPS = 0;
    lastFPSCheck = std::chrono::high_resolution_clock::now();
    particles.reserve(1000);
    explorers.reserve(5);

    if (!font.loadFromFile("../../lib/calibri.ttf")) {
        std::cerr << "Failed to load font file" << std::endl;
    }
}

void SimulationPanel::setServer(void* server) {
    // Not implemented for this example
}

void SimulationPanel::parseJSONToParticles(const json& jsonData) {
    if (jsonData.is_array()) {
        for (const auto& obj : jsonData) {
            double angle = obj.at("angle").get<double>();
            double velocity = obj.at("velocity").get<double>();
            double xcoord = obj.at("xcoord").get<double>();
            double ycoord = obj.at("ycoord").get<double>();

            particles.push_back(std::make_shared<Particle>(xcoord, ycoord, velocity, angle));
        }
    } else {
        double angle = jsonData["angle"];
        double velocity = jsonData["velocity"];
        double xcoord = jsonData["xcoord"];
        double ycoord = jsonData["ycoord"];

        particles.push_back(std::make_shared<Particle>(xcoord, ycoord, velocity, angle));
    }
}

void SimulationPanel::parseJSONToExplorers(const json& jsonData) {
    auto updateOrAddExplorer = [this](int id, double xcoord, double ycoord) {
        auto it = std::find_if(explorers.begin(), explorers.end(),
            [id](const std::shared_ptr<Explorer>& explorer) { return explorer->getID() == id; });

        if (it != explorers.end()) {
            (*it)->updateCoords(xcoord, ycoord);
        } else {
            explorers.push_back(std::make_shared<Explorer>(id, xcoord, ycoord));
        }
    };

    if (jsonData.is_array()) {
        for (const auto& obj : jsonData) {
            int id = obj.at("clientID").get<int>();
            double xcoord = obj.at("xcoord").get<double>();
            double ycoord = obj.at("ycoord").get<double>();

            updateOrAddExplorer(id, xcoord, ycoord);
        }
    } else {
        int id = jsonData["clientID"].get<int>();
        double xcoord = jsonData["xcoord"].get<double>();
        double ycoord = jsonData["ycoord"].get<double>();

        updateOrAddExplorer(id, xcoord, ycoord);
    }

    std::cout << "Explorers size: " << explorers.size() << std::endl;
}

void SimulationPanel::addExplorer(int ID, double x, double y) {
    explorer = std::make_shared<Explorer>(ID, x, y);
    std::cout << "explorer move: " << explorer->getMove() << std::endl;
}

const std::shared_ptr<Explorer>& SimulationPanel::getExplorer() const {
    return explorer;
}

void SimulationPanel::updateSimulation() {
    for (auto& particle : particles) {
        particle->updatePosition(0.1);
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

    for (const auto& others : explorers) {
        target.draw(*others);
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
    }

    target.draw(text);
}

