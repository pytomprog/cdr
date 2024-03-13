#pragma once

#include <SFML/Graphics.hpp>

#include "Vec2f.hpp"

class LineSegment {
public:
	Vec2f m_p1, m_p2;
	Vec2f m_direction;

	sf::Vertex m_shape[2];


	LineSegment(Vec2f p1, Vec2f p2);

	void draw(sf::RenderTarget& renderingSurface);
};