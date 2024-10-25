#include "world/ownRobot/OwnRobot.hpp"

OwnRobot::OwnRobot(Pose2f initialPose, float radius, bool obstacleAvoidance) {
	m_periphericalsConfig = { true, true, true };
	m_cameraConnected = false;

	m_currentPose = initialPose;
	m_currentSpeed = Pose2f(Vec2f(0.f, 0.f), 0.f);
	m_targetPose = Pose2f(Vec2f(0.f, 0.f, sf::Color::Red), 0.f);
	m_commandSpeed = Pose2f(Vec2f(0.f, 0.f), 0.f);
	m_errorPoseIntegrated = Pose2f(Vec2f(0.f, 0.f), 0.f);
	m_radius = radius;
	m_obstacleAvoidance = obstacleAvoidance;
	

	m_motorsEnabled = false;
	m_maxSpeedPercentage = 100;
	m_gamepadMaxSpeedPercentage = 20;
	m_motor1TargetSpeedPercentage = 0;
	m_motor2TargetSpeedPercentage = 0;
	m_motor3TargetSpeedPercentage = 0;
	m_dXTarget = 0;
	m_dYTarget = 0;
	m_dThetaTarget = 0;
	m_gamepadPrimaryAxisSpeedTarget = 0;
	m_gamepadSecondaryAxisSpeedTarget = 0;
	
	m_xyPCoefficient = 1.f;
	m_xyICoefficient = 0.f;
	m_xyDCoefficient = 0.25f;
	m_thetaPCoefficient = 0.5f;
	m_thetaICoefficient = 0.f;
	m_thetaDCoefficient = 0.f;

	m_arm1 = Arm();
	m_arm2 = Arm();
}
