#pragma once

#include <vector>

#include "common/Vec2f.hpp"
#include "common/LineSegment.hpp"
#include "common/Line.hpp"
#include "common/Circle.hpp"

std::vector<LineSegment> findPath(Vec2f startPoint, Vec2f endPoint, std::vector<Circle> obstacles);