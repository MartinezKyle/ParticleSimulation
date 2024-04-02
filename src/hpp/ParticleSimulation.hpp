#ifndef PARTICLE_SIMULATION_HPP
#define PARTICLE_SIMULATION_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "SimulationPanel.hpp"

using json = nlohmann::json;

class ParticleSimulation {
public:
    ParticleSimulation();

    void updateSimulationLoop();
    void run();

    void setID(const json& jsonData);
    bool getIsRunning() const;

    void addParticle(const json& jsonData);
    void addOtherExplorer(const json& jsonData);
    void removeExplorer(const json& jsonData);

    SimulationPanel& getSimulationPanel();

private:
    SimulationPanel simulationPanel;
    std::atomic<bool> isRunning = true;
    int ID = -1;
};

#endif // PARTICLE_SIMULATION_HPP
