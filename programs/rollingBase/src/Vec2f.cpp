#include "Vec2f.hpp"

#include "parameters.hpp"

Vec2f::Vec2f(float x, float y, sf::Color color) {
	m_x = x;
	m_y = y;

	m_shape = sf::CircleShape(POINTS_RADIUS * ZOOM, POINTS_SHAPE_EDGES_COUNT);
	m_shape.setOrigin(POINTS_RADIUS * ZOOM, POINTS_RADIUS * ZOOM);
	m_shape.setPosition(x * ZOOM, y * ZOOM);
	m_shape.setFillColor(color);
}

Vec2f Vec2f::operator-() { return Vec2f(-m_x, -m_y); }
Vec2f Vec2f::operator+(Vec2f otherVec) { return Vec2f(m_x + otherVec.m_x, m_y + otherVec.m_y); }
Vec2f Vec2f::operator-(Vec2f otherVec) { return Vec2f(m_x - otherVec.m_x, m_y - otherVec.m_y); }
Vec2f Vec2f::operator*(float nb) { return Vec2f(m_x * nb, m_y * nb); }
Vec2f Vec2f::operator/(float nb) { return Vec2f(m_x / nb, m_y / nb); }
void Vec2f::operator+=(Vec2f otherVec) {
	m_x += otherVec.m_x;
	m_y += otherVec.m_y;
}
void Vec2f::operator-=(Vec2f otherVec) {
	m_x -= otherVec.m_x;
	m_y -= otherVec.m_y;
}
Vec2f operator*(double nb, Vec2f vec) { return vec * nb; }

sf::Vector2f Vec2f::toSfmlVector() {
	return sf::Vector2f(m_x, m_y);
}

void Vec2f::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape);
}