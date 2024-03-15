#pragma once

#include "Vec2f.hpp"

#include "Line.hpp"

#include <vector>
#include <SFML/Graphics.hpp>

class Circle {
public:
	Vec2f m_center;
	float m_radius;

	sf::CircleShape m_shape;


	Circle(Vec2f center, float radius, sf::Color color = sf::Color::Magenta);

	std::vector<Vec2f> getPointsOfTangentLines(Vec2f p);

	void draw(sf::RenderTarget& renderingSurface);
};