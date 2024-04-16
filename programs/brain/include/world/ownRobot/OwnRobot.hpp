#pragma once

#include "maths/Pose2f.hpp"

struct OwnRobotPeriphericalsConfig {
	bool cameraConnected;
	bool rollingBaseConnected;
	bool armConnected;
};

enum OwnRobotRollingBaseMode {
	INDIVIDUAL_MOTOR_SPEEDS_CONTROL,
	DX_DY_DTHETA_CONTROL,
	POSE_CONTROL,
};

class OwnRobot {
public:
	OwnRobotPeriphericalsConfig m_periphericalsConfig;

	Pose2f m_currentPose;
	Pose2f m_targetPose;

	OwnRobotRollingBaseMode m_ownRobotRollingBaseMode;
	bool m_motorsEnabled;
	int m_motor1TargetSpeedPercentage;
	int m_motor2TargetSpeedPercentage;
	int m_motor3TargetSpeedPercentage;
	int m_dXTarget;
	int m_dYTarget;
	int m_dThetaTarget;

	//Arm arm;

	OwnRobot(Pose2f initialPose = Pose2f());
};