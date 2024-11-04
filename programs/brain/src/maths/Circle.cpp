#include "maths/Circle.hpp"

#include <cmath>

#include "parameters.hpp"

Circle::Circle(Vec2f center, float selfRadius, float robotRadius, sf::Color color) {
	m_center = center;
	m_selfRadius = selfRadius;
	m_collisionRadius = selfRadius + robotRadius;

	m_selfShape = sf::CircleShape(m_collisionRadius * ZOOM, CIRCLES_SHAPE_EDGES_COUNT);
	m_selfShape.setOrigin(m_collisionRadius * ZOOM, m_collisionRadius * ZOOM);
	m_selfShape.setPosition(center.toSfmlVector());
	m_selfShape.setFillColor(color);

	m_collisionShape = sf::CircleShape(m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS, CIRCLES_SHAPE_EDGES_COUNT);
	m_collisionShape.setOrigin(m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS, m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS);
	m_collisionShape.setPosition(center.toSfmlVector());
	m_collisionShape.setFillColor(sf::Color::Transparent);
	m_collisionShape.setOutlineThickness(CIRCLE_OUTLINE_THICKNESS);
	m_collisionShape.setOutlineColor(color);
}

std::array<Vec2f, 2> Circle::getPointsOfTangentLines(Vec2f p) {
	float distance = (p - m_center).getNorm();
	float arg = (p - m_center).getArgument();
	float theta = std::acos(1 / (distance / m_collisionRadius));
	return std::array<Vec2f, 2> {{ m_center + Vec2f(m_collisionRadius * cos(arg + theta), m_collisionRadius * sin(arg + theta)), m_center + Vec2f(m_collisionRadius * cosf(arg - theta), m_collisionRadius * sin(arg - theta)) }};
} 

void Circle::update(float robotRadius) { //TODO: Change name because it updates radius also
	m_collisionRadius = m_selfRadius + robotRadius;
	
	m_center.updatePosition();

	m_selfShape.setPosition(m_center.toSfmlVector());
	m_selfShape.setRadius(m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS);
	m_selfShape.setOrigin(m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS, m_selfRadius * ZOOM - CIRCLE_OUTLINE_THICKNESS);

	m_collisionShape.setPosition(m_center.toSfmlVector());
	m_collisionShape.setRadius(m_collisionRadius * ZOOM);
	m_collisionShape.setOrigin(m_collisionRadius * ZOOM, m_collisionRadius * ZOOM);
}


void Circle::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_selfShape);
	renderingSurface.draw(m_collisionShape);
}
