#ifndef UTILS_HPP
#define UTILS_HPP

#include <Arduino.h>

inline int clamp(int value, int minValue, int maxValue) { return min(max(value, minValue), maxValue); }
inline float clamp(float value, float minValue, float maxValue) { return min(max(value, minValue), maxValue); }

float wrapToPi(float angle);

#endif //UTILS_HPP