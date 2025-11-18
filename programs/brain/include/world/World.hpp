#pragma once

#include "world/ownRobot/OwnRobot.hpp"
#include "world/obstacle/Obstacle.hpp"

#include <chrono>

enum RunningMode {
	TEST_MODE,
	STRATEGY_MODE,
};

class World {
public:
	RunningMode m_runningMode;
	std::string m_strategyName;
	int m_strategyStep;
	
	bool started;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

	OwnRobot& m_ownRobot;
	Obstacle& m_advRobot;

	World(RunningMode runningMode, std::string strategyName, OwnRobot& ownRobot, Obstacle& advRobot);
};
