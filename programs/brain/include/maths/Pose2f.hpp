#pragma once

#include "maths/Vec2f.hpp"

class Pose2f: Vec2f {
public:
	Vec2f m_position;
	float m_orientation;

	// TODO: Add orientation shape 

	Pose2f(Vec2f m_position = Vec2f(), float orientation = 0.f);

	void update();
	void draw(sf::RenderTarget& renderingSurface);
};