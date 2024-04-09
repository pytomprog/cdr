#pragma once

#include "Vec2f.hpp"

#include "Line.hpp"

#include <array>
#include <SFML/Graphics.hpp>

class Circle {
public:
	Vec2f m_center;
	float m_selfRadius;
	float m_collisionRadius;

	sf::CircleShape m_selfShape;
	sf::CircleShape m_collisionShape;


	Circle(Vec2f center, float radius, float robotRadius, sf::Color color = sf::Color::Magenta);

	std::array<Vec2f, 2> getPointsOfTangentLines(Vec2f p);
	
	void update(float robotRadius);
	void draw(sf::RenderTarget& renderingSurface);
};
