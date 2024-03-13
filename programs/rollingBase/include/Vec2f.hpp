#pragma once

#include <SFML/Graphics.hpp>

class Vec2f {
public:
	float m_x, m_y;

	sf::CircleShape m_shape;


	Vec2f(float x = 0.f, float y = 0.f, sf::Color color = sf::Color::Green);

	Vec2f operator*(float nb);
	Vec2f operator/(float nb);
	Vec2f operator+(Vec2f otherVec);
	Vec2f operator-(Vec2f otherVec);
	Vec2f operator-();
	void operator+=(Vec2f otherVec);
	void operator-=(Vec2f otherVec);

	sf::Vector2f toSfmlVector();

	void draw(sf::RenderTarget& renderingSurface);
};