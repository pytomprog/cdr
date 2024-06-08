#include "world/obstacle/Obstacle.hpp"

Obstacle::Obstacle(Pose2f initialPose, float radius) {
	m_currentPose = initialPose;
	m_radius = radius;
};