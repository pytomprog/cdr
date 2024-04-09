#pragma once

#include <vector>

#include "maths/Vec2f.hpp"
#include "maths/LineSegment.hpp"
#include "maths/Line.hpp"
#include "maths/Circle.hpp"

std::vector<LineSegment> findPath(Vec2f startPoint, Vec2f endPoint, std::vector<Circle> obstacles);