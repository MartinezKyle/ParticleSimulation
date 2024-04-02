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

    void setID(const json& jsonData);

    void addParticle(const json& jsonData);

    void addOtherExplorer(const json& jsonData);

    SimulationPanel& getSimulationPanel();

private:
    SimulationPanel simulationPanel;
    int ID = -1;
};

#endif // PARTICLE_SIMULATION_HPP
