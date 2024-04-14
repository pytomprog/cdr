#pragma once

#include "world/ownRobot/OwnRobot.hpp"

class World {
public:
	OwnRobot& m_ownRobot;

	World(OwnRobot& ownRobot);
};