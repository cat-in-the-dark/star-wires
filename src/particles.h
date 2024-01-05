#pragma once

#include <raylib.h>

#include <vector>

struct Triangle {
  Vector3 vertices[3];
  Vector3 dir;
};

struct Particles {
  std::vector<Triangle> triangles;

  void Draw(Vector3 pos, float rotationAngle, Vector3 rotationAxis, float time, float speed, float lifetime);
};

float CalculateMeanDistance(Model model);

Particles MeshTriangleSplit(Model model, bool randomize);
