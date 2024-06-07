#pragma once

#include "world/ownRobot/OwnRobot.hpp"
#include "world/obstacle/Obstacle.hpp"

enum RunningMode {
	TEST_MODE,
	STRATEGY_MODE,
};

class World {
public:
	RunningMode m_runningMode;
	std::string m_strategyName;
	int m_strategyStep;

	OwnRobot& m_ownRobot;
	Obstacle& m_advRobot;

	World(RunningMode runningMode, std::string strategyName, OwnRobot& ownRobot, Obstacle& advRobot);
};