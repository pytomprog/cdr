#pragma once

#include <vector>

#include "Vec2f.hpp"
#include "LineSegment.hpp"
#include "Line.hpp"
#include "Circle.hpp"

std::vector<LineSegment> findPath(Vec2f startPoint, Vec2f endPoint, std::vector<Circle> obstacles);