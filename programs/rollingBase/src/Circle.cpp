#include "Circle.hpp"

#include "parameters.hpp"

Circle::Circle(Vec2f center, float radius, sf::Color color) {
	m_center = center;
	m_radius = radius;

	m_shape = sf::CircleShape(radius * ZOOM - CIRCLE_OUTLINE_THICKNESS, CIRCLES_SHAPE_EDGES_COUNT);

	m_shape.setOrigin(radius * ZOOM - CIRCLE_OUTLINE_THICKNESS, radius * ZOOM - CIRCLE_OUTLINE_THICKNESS);
	m_shape.setPosition((center * ZOOM).toSfmlVector());
	m_shape.setFillColor(sf::Color::Transparent);
	m_shape.setOutlineThickness(CIRCLE_OUTLINE_THICKNESS);
	m_shape.setOutlineColor(color);
}

void Circle::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape);
}