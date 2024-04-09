#include "maths/Pose2f.hpp"

Pose2f::Pose2f(Vec2f position, float orientation) {
	m_position = position;
	m_orientation = orientation;
}

void Pose2f::update() {
	m_position.updatePosition();
}

void Pose2f::draw(sf::RenderTarget& renderingSurface) {
	m_position.draw(renderingSurface);
}