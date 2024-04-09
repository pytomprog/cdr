#pragma once

#include "maths/Pose2f.hpp"

class Robot {
public:
	Pose2f m_currentPose;
	Pose2f m_targetPose = Pose2f(Vec2f(0.f, 0.f, sf::Color::Red), 0.f);
	
	//Arm arm;

	Robot(Pose2f initialPose = Pose2f());
};