#include "world/ownRobot/OwnRobot.hpp"

OwnRobot::OwnRobot(Pose2f initialPose) {
	m_periphericalsConfig = { false, false, false };

	m_currentPose = initialPose;
	m_targetPose = Pose2f(Vec2f(0.f, 0.f, sf::Color::Red), 0.f);

	m_motorsEnabled = true;
	m_motor1TargetSpeedPercentage = 0;
	m_motor2TargetSpeedPercentage = 0;
	m_motor3TargetSpeedPercentage = 0;
	m_dXTarget = 0;
	m_dYTarget = 0;
	m_dThetaTarget = 0;
}