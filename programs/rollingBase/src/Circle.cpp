#include "Circle.hpp"

#include <cmath>

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
	float theta = std::acos(1 / (distance / m_radius));
	return std::array<Vec2f, 2> {{ m_center + Vec2f(m_radius * cos(arg + theta), m_radius * sin(arg + theta)), m_center + Vec2f(m_radius * cosf(arg - theta), m_radius * sin(arg - theta)) }};
} 

void Circle::updatePosition() { //TODO: Change name because it updates radius also
	m_center.updatePosition();
	m_shape.setPosition(m_center.m_x * ZOOM, m_center.m_y * ZOOM);
	m_shape.setRadius(m_radius * ZOOM - CIRCLE_OUTLINE_THICKNESS);
	m_shape.setOrigin(m_radius * ZOOM - CIRCLE_OUTLINE_THICKNESS, m_radius * ZOOM - CIRCLE_OUTLINE_THICKNESS);
}


void Circle::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape);
}
