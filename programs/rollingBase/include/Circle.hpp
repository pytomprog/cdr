#pragma once

#include "Vec2f.hpp"

#include <SFML/Graphics.hpp>

class Circle {
public:
	Vec2f m_center;
	float m_radius;

	sf::CircleShape m_shape;


	Circle(Vec2f center, float radius, sf::Color color = sf::Color::White);

	void draw(sf::RenderTarget& renderingSurface);
};