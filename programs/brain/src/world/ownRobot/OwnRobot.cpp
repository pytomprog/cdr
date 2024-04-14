#include "world/ownRobot/OwnRobot.hpp"

OwnRobot::OwnRobot(Pose2f initialPose) {
	m_currentPose = initialPose;
	m_periphericalsConfig = { false, false, false };
}