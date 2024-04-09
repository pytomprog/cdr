#pragma once

#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include "maths/Vec2f.hpp"
#include "maths/LineSegment.hpp"
#include "maths/Circle.hpp"

struct Color4f {
    // [0-255]
    float r,g,b,a;
};

class DebugGui {
public:
	sf::Clock m_deltaClock;
	Color4f m_obstaclesColor{1.f, 0.f, 1.f, 1.f};

    void update(sf::RenderWindow& window, Vec2f& startPoint, Vec2f& endPoint, float& robotRadius, std::vector<LineSegment>& path, std::vector<Circle>& obstacles);
};
