#include "mymath.h"

#include <raymath.h>

float GetRandomFloat() {
  return static_cast<float>(GetRandomValue(0, 1000)) / 1000.0;
}

float GetRandomNegOneToOne() {
  return (GetRandomFloat() - 0.5) * 2;
}

Vector3 Vector3Random(float v) {
  Vector3 vec;
  vec.x = 2 * (GetRandomFloat() - 0.5) * v;
  vec.y = 2 * (GetRandomFloat() - 0.5) * v;
  vec.z = 2 * (GetRandomFloat() - 0.5) * v;
  return vec;
}
