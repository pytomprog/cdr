#pragma once

#include "maths/Pose2f.hpp"

struct OwnRobotPeriphericalsConfig {
	bool cameraConnected;
	bool rollingBaseConnected;
	bool armConnected;
};

class OwnRobot {
public:
	Pose2f m_currentPose;
	Pose2f m_targetPose = Pose2f(Vec2f(0.f, 0.f, sf::Color::Red), 0.f);

	OwnRobotPeriphericalsConfig m_periphericalsConfig;

	//Arm arm;

	OwnRobot(Pose2f initialPose = Pose2f());
};