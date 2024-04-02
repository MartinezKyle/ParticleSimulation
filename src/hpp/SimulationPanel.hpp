#ifndef SIMULATION_PANEL_H
#define SIMULATION_PANEL_H

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

class SimulationPanel : public sf::Drawable, public sf::Transformable {
public:
    SimulationPanel(bool isDev);
    void setServer(void* server);
    void parseJSONToParticles(const json& jsonData);
    void parseJSONToExplorers(const json& jsonData);
    void addExplorer(int ID, double x, double y);
    const std::shared_ptr<Explorer>& getExplorer() const;
    void updateSimulation();
    void changeDevMode(bool isDev);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<std::shared_ptr<Explorer>> explorers;
    std::shared_ptr<Explorer> explorer;
    bool isDevMode;
    int frameCount;
    int previousFPS;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFPSCheck;
    sf::Font font;

    void drawFPSInfo(sf::RenderTarget& target) const;
};

#endif // SIMULATION_PANEL_H
