#include "Line.hpp"

#include <iostream>

#include "parameters.hpp"

Line::Line(Vec2f direction, Vec2f p, sf::Color color) {
	m_direction = direction;
	m_p = p;

	m_a = direction.m_y;
	m_b = -direction.m_x;
	m_c = -(m_a * p.m_x + m_b * p.m_y);

	Vec2f p1, p2;
	p1 = getPointWithX(X_MIN_BORDER);
	if (p1.m_y < Y_MIN_BORDER)
		p1 = getPointWithY(Y_MIN_BORDER);
	else if (p1.m_y > Y_MAX_BORDER) {
		p1 = getPointWithY(Y_MAX_BORDER);
	}
	p2 = getPointWithX(X_MAX_BORDER);
	if (p2.m_y < Y_MIN_BORDER)
		p2 = getPointWithY(Y_MIN_BORDER);
	else if (p2.m_y > Y_MAX_BORDER) {
		p2 = getPointWithY(Y_MAX_BORDER);
	}

	m_shape[0] = sf::Vertex(p1.toSfmlVector() * ZOOM, color);
	m_shape[1] = sf::Vertex(p2.toSfmlVector() * ZOOM, color);
}

Line::Line(LineSegment segment, sf::Color color): Line(segment.m_direction.normalized(), segment.m_p1, color) {}

float Line::getY(float x) {
	// a*x + b*y + c = 0 so y = -(a*x + c) / b

	if (m_b == 0.f) {
		std::cerr << "Infinity of y values" << std::endl;
		//return NAN;
	}

	return -(m_a * x + m_c) / m_b;
}

float Line::getX(float y) {
	// a*x + b*y + c = 0 so x = -(b*y + c) / a

	if (m_a == 0.f) {
		std::cerr << "Infinity of x values" << std::endl;
		//return NAN;
	}

	return -(m_b * y + m_c) / m_a;
}

Vec2f Line::getPointWithX(float x) {
	return Vec2f(x, getY(x));
}

Vec2f Line::getPointWithY(float y) {
	return Vec2f(getX(y), y);
}

Line Line::getPerpendicularLineSegment(Vec2f p) {
	return Line(Vec2f(m_a, m_b), p);
}

void Line::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape, 2, sf::Lines);
}

Vec2f getCollisionPoint(Line l1, Line l2) {
	if (l1.m_a * l2.m_b - l1.m_b * l2.m_a == 0.f) {
		std::cerr << "No collision point" << std::endl;
		return Vec2f(NAN, NAN);
	}

	float x = (l1.m_b * l2.m_c - l1.m_c * l2.m_b) / (l1.m_a * l2.m_b - l1.m_b * l2.m_a);
	return Vec2f(x, l1.getY(x));
}