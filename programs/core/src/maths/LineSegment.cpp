#include "maths/LineSegment.hpp"

#include "cmath"

#include "parameters.hpp"

LineSegment::LineSegment(Vec2f p1, Vec2f p2, sf::Color color) {
	m_p1 = p1;
	m_p2 = p2;
	m_direction = p2 - p1;

	m_shape[0] = sf::Vertex(m_p1.toSfmlVector() * ZOOM, color);
	m_shape[1] = sf::Vertex(m_p2.toSfmlVector() * ZOOM, color);
}

bool LineSegment::containsPoint(Vec2f p) {
	if (LineSegment(m_p1, p).m_direction.getNorm() + LineSegment(m_p2, p).m_direction.getNorm() - m_direction.getNorm() < LINE_SEGMENT_CONTAINING_POINT_DISTANCE_THRESHOLD) {
		return true;
	}
	return false;
}

void LineSegment::draw(sf::RenderTarget& renderingSurface) {
	renderingSurface.draw(m_shape, 2, sf::Lines);
}