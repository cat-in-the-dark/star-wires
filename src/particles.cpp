#include "particles.h"

#include <raymath.h>
#include <rlgl.h>

#include "mymath.h"
#include "renderer.h"

Vector3 mean3(float* normals, int ti) {
  Vector3 v = {0};
  for (int i = 0; i < 3; i++) {
    v.x += normals[(ti + i) * 3];
    v.y += normals[(ti + i) * 3 + 1];
    v.z += normals[(ti + i) * 3 + 2];
  }
  v.x /= 3.0;
  v.y /= 3.0;
  v.z /= 3.0;
  return v;
}

Particles MeshTriangleSplit(Model model, bool randomize) {
  Mesh mesh = model.meshes[0];

  Particles particles;

  for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
    Triangle tri;
    tri.dir = mean3(mesh.normals, i);
    if (randomize) {
      tri.dir = Vector3Add(tri.dir, Vector3Random(0.2));
    }
    // TraceLog(LOG_INFO, "%f %f %f", tri.dir.x, tri.dir.y, tri.dir.z);

    for (int vi = 0; vi < 3; vi++) {
      tri.vertices[vi].x = mesh.vertices[3 * (i + vi)];
      tri.vertices[vi].y = mesh.vertices[3 * (i + vi) + 1];
      tri.vertices[vi].z = mesh.vertices[3 * (i + vi) + 2];
    }

    particles.triangles.push_back(tri);
  }

  return particles;
}

void Particles::Draw(Vector3 pos, float rotationAngle, Vector3 rotationAxis, float time, float speed, float lifetime) {
  auto color = RED;
  float t = Clamp(time / lifetime, 0.0, 1.0);
  color.a = Lerp(255, 0, t);
  color.b = Clamp(Lerp(0, 300, t), 0, 255);

  if (t >= 1) {
    return;
  }

  rlPushMatrix();
  rlTranslatef(pos.x, pos.y, pos.z);
  rlRotatef(rotationAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
  for (auto tri : triangles) {
    rlPushMatrix();
    auto pos = Vector3Scale(tri.dir, speed * time);
    rlTranslatef(pos.x, pos.y, pos.z);
    DrawLineTriangle3DCull(tri.vertices[0], tri.vertices[1], tri.vertices[2], color);
    rlPopMatrix();
  }
  rlPopMatrix();
}