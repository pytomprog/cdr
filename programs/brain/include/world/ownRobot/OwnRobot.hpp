#pragma once

#include "maths/Pose2f.hpp"
#include "world/ownRobot/arm/Arm.hpp"

struct OwnRobotPeriphericalsConfig {
	bool cameraEnabled;
	bool rollingBaseEnabled;
	bool armEnabled;
};

enum OwnRobotRollingBaseMode {
	INDIVIDUAL_MOTOR_SPEEDS_CONTROL,
	DX_DY_DTHETA_CONTROL,
	GAMEPAD_CONTROL,
	POSE_CONTROL,
};

class OwnRobot {
public:
	OwnRobotPeriphericalsConfig m_periphericalsConfig;
	
	bool m_cameraConnected;

	Pose2f m_currentPose;
	Pose2f m_currentSpeed;
	Pose2f m_targetPose;
	Pose2f m_commandSpeed;
	Pose2f m_errorPoseIntegrated;
	float m_radius;
	bool m_obstacleAvoidance;

	OwnRobotRollingBaseMode m_ownRobotRollingBaseMode;
	bool m_motorsEnabled;
	int m_maxSpeedPercentage;
	int m_gamepadMaxSpeedPercentage;
	int m_motor1TargetSpeedPercentage;
	int m_motor2TargetSpeedPercentage;
	int m_motor3TargetSpeedPercentage;
	int m_dXTarget;
	int m_dYTarget;
	int m_dThetaTarget;
	int m_gamepadPrimaryAxisSpeedTarget;
	int m_gamepadSecondaryAxisSpeedTarget;
	
	float m_xyPCoefficient, m_xyICoefficient, m_xyDCoefficient;
	float m_thetaPCoefficient, m_thetaICoefficient, m_thetaDCoefficient;

	Arm m_arm1, m_arm2;

	OwnRobot(Pose2f initialPose = Pose2f(), float radius = 30.f, bool obstacleAvoidance = false);
};
