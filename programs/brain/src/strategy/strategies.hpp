#pragma once

#ifndef STRATEGIES
#define STRATEGIES

#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "world/World.hpp"

#include "nodalStrategy/DebugGui.hpp"
#include "nodalStrategy/StrategyStateMachine.hpp"
#include "nodalStrategy/StrategyStates.hpp"

class Strategy {
public:
    World& m_world;
    
    virtual void routine() = 0;
    Strategy(World& world);
    
    virtual void guiRoutine(sf::RenderWindow& window) {}; //Handle it correctly, outside of this file 
};

void initStrategies(std::map<std::string, Strategy*>& strategies, std::vector<std::string>& strategiesNames, World& world);

class BlueHomologation : public Strategy { public: BlueHomologation(World& world); void routine(); };
class YellowHomologation : public Strategy { public: YellowHomologation(World& world); void routine(); };
class ArmTest : public Strategy { public: ArmTest(World& world); void routine(); };
class Demo : public Strategy { public: Demo(World& world); void routine(); };

class NodalStrategy : public Strategy {
    public:
    StrategyStateMachine m_stateMachine;
    DebugGui m_debugGui;
    
    NodalStrategy(World& world);
    void routine();
    void guiRoutine(sf::RenderWindow& window) override;
};

#endif //STRATEGIES
