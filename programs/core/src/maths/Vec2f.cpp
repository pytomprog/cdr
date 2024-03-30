#include "maths/Vec2f.hpp"

#include <cmath>

#include "parameters.hpp"

Vec2f::Vec2f(float x, float y, sf::Color color) {
	m_x = x;
	m_y = y;

	m_shape = sf::CircleShape(POINTS_RADIUS * ZOOM, POINTS_SHAPE_EDGES_COUNT);
	m_shape.setOrigin(POINTS_RADIUS * ZOOM, POINTS_RADIUS * ZOOM);
	updatePosition();
	m_shape.setFillColor(color);
}

Vec2f operator*(double nb, Vec2f vec) { return vec * nb; }
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

float Vec2f::getNorm() {
	return std::sqrt(m_x * m_x + m_y * m_y);
}

float Vec2f::getArgument() {
	return std::atan2(m_y, m_x);
}

Vec2f Vec2f::normalized() {
	return (*this)/getNorm();
}

Vec2f Vec2f::getNearestPoint(Vec2f& otherVec1, Vec2f& otherVec2) {
	return (getDistance(*this, otherVec1) <= getDistance(*this, otherVec2)) ? otherVec1 : otherVec2;;
}

void Vec2f::updatePosition() {
	m_shape.setPosition(m_x * ZOOM, m_y * ZOOM);
}

sf::Vector2f Vec2f::toSfmlVector() {
	return sf::Vector2f(m_x, m_y);
}

void Vec2f::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape);
}

float getDistance(Vec2f vec1, Vec2f vec2) {
	return std::sqrt((vec2.m_x - vec1.m_x) * (vec2.m_x - vec1.m_x) + (vec2.m_y - vec1.m_y) * (vec2.m_y - vec1.m_y));
}

float dotProduct(Vec2f vec1, Vec2f vec2) {
	return vec1.m_x * vec2.m_x + vec1.m_y * vec2.m_y;
}
