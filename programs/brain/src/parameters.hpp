#pragma once

#ifndef PARAMETERS
#define PARAMETERS

//===================== General parameters =====================

#define VERBOSE

constexpr float PI = 3.14159265358979;

constexpr float X_MIN_BORDER = 0.f, X_MAX_BORDER = 3000.f, Y_MIN_BORDER = 0.f, Y_MAX_BORDER = 2000.f;

constexpr unsigned int WIDTH = 1500u, HEIGHT = 1000u;

constexpr float ZOOM = 0.5f;

constexpr float POINTS_RADIUS = 10.f;
constexpr unsigned int POINTS_SHAPE_EDGES_COUNT = 30u;

constexpr float LINE_SEGMENT_CONTAINING_POINT_DISTANCE_THRESHOLD = 0.01f;
constexpr float LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD = 0.01f;

constexpr unsigned int CIRCLES_SHAPE_EDGES_COUNT = 100u;
constexpr float CIRCLE_OUTLINE_THICKNESS = 2.f;

#define PIN_TIRRETE 15

#define MAX_STRATEGIES_NB 256

//======================= HAL parameters =======================

#define I2C_CLOCK_DIVIDER_VALUE BCM2835_I2C_CLOCK_DIVIDER_2500 //BCM2835_I2C_CLOCK_DIVIDER_148 too fast without pullup resistors (and with ?)

#define I2C_ROLLING_BASE_ADDRESS 0x10

#define I2C_ARM_1_ADDRESS 0x11
#define I2C_ARM_2_ADDRESS 0x12

//=================== Pathfinding parameters ===================

constexpr float MAX_OBSTACLE_RADIUS = 1000.f;

#endif //PARAMETERS
