#pragma once

#include "maths/Pose2f.hpp"

class Obstacle {
public:
	Pose2f m_currentPose;
    float m_radius;

	Obstacle(Pose2f initialPose = Pose2f(), float radius = 30.f);
};