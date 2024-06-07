#pragma once

#ifndef STRATEGIES
#define STRATEGIES

#include <map>
#include <string>

#include "world/World.hpp"

class Strategy {
public:
    World& m_world;

    virtual void routine() = 0;
    Strategy(World& world);
};

void initStrategies(std::map<std::string, Strategy*>& strategies, std::vector<std::string>& strategiesNames, World& world);

class BlueHomologation : public Strategy { public: BlueHomologation(World& world); void routine(); };
class YellowHomologation : public Strategy { public: YellowHomologation(World& world); void routine(); };
class ArmTest : public Strategy { public: ArmTest(World& world); void routine(); };

#endif //STRATEGIES