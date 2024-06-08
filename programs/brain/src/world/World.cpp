#include "world/World.hpp"

World::World(RunningMode runningMode, std::string strategyName, OwnRobot& ownRobot, Obstacle& advRobot): m_ownRobot(ownRobot), m_advRobot(advRobot) {
    m_runningMode = runningMode;
    m_strategyName = strategyName;
    m_strategyStep = 0;
}