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

std::array<Vec2f, 2> Circle::getPointsOfTangentLines(Vec2f p) {
	float distance = (p - m_center).getNorm();
	float arg = (p - m_center).getArgument();
	float theta = std::acosf(1 / (distance / m_radius));
	return std::array<Vec2f, 2> {{ m_center + Vec2f(m_radius * cosf(arg + theta), m_radius * sinf(arg + theta)), m_center + Vec2f(m_radius * cosf(arg - theta), m_radius * sinf(arg - theta)) }};
} 

void Circle::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape);
}