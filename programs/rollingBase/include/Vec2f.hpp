#pragma once

#include <SFML/Graphics.hpp>

class Vec2f {
public:
	float m_x, m_y;

	sf::CircleShape m_shape;


	Vec2f(float x = 0.f, float y = 0.f, sf::Color color = sf::Color::Blue);

	Vec2f operator-();
	Vec2f operator+(Vec2f otherVec);
	Vec2f operator-(Vec2f otherVec);
	Vec2f operator*(float nb);
	Vec2f operator/(float nb);
	void operator+=(Vec2f otherVec);
	void operator-=(Vec2f otherVec);

	float getNorm();
	float getArgument();
	Vec2f normalized();

	sf::Vector2f toSfmlVector();

	void draw(sf::RenderTarget& renderingSurface);
};

float dotProduct(Vec2f vec1, Vec2f vec2);