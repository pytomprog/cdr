#pragma once

#define VERBOSE

constexpr float X_MIN_BORDER = 0.f, X_MAX_BORDER = 3000.f, Y_MIN_BORDER = 0.f, Y_MAX_BORDER = 2000.f;

constexpr float MAX_OBSTACLE_RADIUS = 1000.f;

constexpr unsigned int WIDTH = 1500, HEIGHT = 1000;

constexpr float ZOOM = 0.5f;

constexpr float POINTS_RADIUS = 10;
constexpr unsigned int POINTS_SHAPE_EDGES_COUNT = 30;

constexpr float LINE_SEGMENT_CONTAINING_POINT_DISTANCE_THRESHOLD = 0.01;
constexpr float LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD = 0.01;

constexpr unsigned int CIRCLES_SHAPE_EDGES_COUNT = 100;
constexpr float CIRCLE_OUTLINE_THICKNESS = 2.f;
