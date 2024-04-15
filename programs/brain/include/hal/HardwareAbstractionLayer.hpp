#pragma once

#include "world/World.hpp"

class HardwareAbstractionLayer {
public:
	World& m_world;

	HardwareAbstractionLayer(World& world);

	void ownRobotRollingBaseRoutine();
};