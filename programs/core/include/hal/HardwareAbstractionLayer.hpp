#pragma once

#include "hal/robot/Robot.hpp"

struct HardwareAbstractionLayerConfig {
	bool cameraConnected;
	bool rollingBaseConnected;
	bool armConnected;
};

class HardwareAbstractionLayer {
public:
	Robot m_ownRobot;

	HardwareAbstractionLayerConfig m_config;

	HardwareAbstractionLayer(Robot const& robot);

	void updateConfig();

	void cameraRoutine();
	void rollingBaseRoutine();
	void armRoutine();
};