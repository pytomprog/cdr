#pragma once

//===================== General parameters =====================

#define VERBOSE

constexpr float X_MIN_BORDER = 0.f, X_MAX_BORDER = 3000.f, Y_MIN_BORDER = 0.f, Y_MAX_BORDER = 2000.f;

constexpr unsigned int WIDTH = 1500u, HEIGHT = 1000u;

constexpr float ZOOM = 0.5f;

constexpr float POINTS_RADIUS = 10.f;
constexpr unsigned int POINTS_SHAPE_EDGES_COUNT = 30u;

constexpr float LINE_SEGMENT_CONTAINING_POINT_DISTANCE_THRESHOLD = 0.01f;
constexpr float LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD = 0.01f;

constexpr unsigned int CIRCLES_SHAPE_EDGES_COUNT = 100u;
constexpr float CIRCLE_OUTLINE_THICKNESS = 2.f;

//=================== Pathfinding parameters ===================

constexpr float MAX_OBSTACLE_RADIUS = 1000.f;
