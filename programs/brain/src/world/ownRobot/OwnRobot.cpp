#include "world/ownRobot/OwnRobot.hpp"

OwnRobot::OwnRobot(Pose2f initialPose, float radius, bool obstacleAvoidance) {
	m_periphericalsConfig = { true, true, true };

	m_currentPose = initialPose;
	m_targetPose = Pose2f(Vec2f(0.f, 0.f, sf::Color::Red), 0.f);
	m_radius = radius;
	m_obstacleAvoidance = obstacleAvoidance;

	m_motorsEnabled = false;
	m_maxSpeedPercentage = 10;
	m_motor1TargetSpeedPercentage = 0;
	m_motor2TargetSpeedPercentage = 0;
	m_motor3TargetSpeedPercentage = 0;
	m_dXTarget = 0;
	m_dYTarget = 0;
	m_dThetaTarget = 0;

	m_arm1 = Arm();
	m_arm2 = Arm();
}