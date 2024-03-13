#pragma once

#include "Vec2f.hpp"

#include <SFML/Graphics.hpp>

class LineSegment {
public:
	Vec2f m_p1, m_p2;
	Vec2f m_direction;

	sf::Vertex m_shape[2];


	LineSegment(Vec2f p1, Vec2f p2, sf::Color color = sf::Color::White);

	void draw(sf::RenderTarget& renderingSurface);
};