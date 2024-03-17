#pragma once

#include "Vec2f.hpp"
#include "LineSegment.hpp"

#include <SFML/Graphics.hpp>

class Line {
public:
	Vec2f m_direction;
	Vec2f m_p;

	float m_a, m_b, m_c; // a*x + b*y + c = 0

	sf::Vertex m_shape[2];


	Line(Vec2f direction = Vec2f(0.f, 0.f), Vec2f pVec2f = Vec2f(0.f, 0.f), sf::Color color = sf::Color::White);
	Line(LineSegment segment, sf::Color color = sf::Color::White);

	float getY(float x);
	float getX(float y);
	Vec2f getPointWithX(float x);
	Vec2f getPointWithY(float y);
	Line getPerpendicularLineSegment(Vec2f p);

	void draw(sf::RenderTarget& renderingSurface);
};

Vec2f getCollisionPoint(Line l1, Line l2);