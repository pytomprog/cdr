#include "utils.hpp"

float wrapToPi(float angle) {
  angle = fmod(angle + PI, TWO_PI);
  if (angle < 0)
    angle += TWO_PI;
  return angle - PI;
}