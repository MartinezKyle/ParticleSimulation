#ifndef PARTICLE_SIMULATION_HPP
#define PARTICLE_SIMULATION_HPP

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "SimulationPanel.hpp"

using json = nlohmann::json;

class ParticleSimulation {
public:
    ParticleSimulation();

    void run();

    void addParticle(const json& jsonData);

    SimulationPanel& getSimulationPanel();

private:
    SimulationPanel simulationPanel;
};

#endif // PARTICLE_SIMULATION_HPP
