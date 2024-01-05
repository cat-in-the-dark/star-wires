#include "particles.h"

#include <rlgl.h>

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

Particles MeshTriangleSplit(Model model) {
  Mesh mesh = model.meshes[0];

  Particles particles;

  for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
    Triangle tri;
    tri.dir = mean3(mesh.normals, i);

    for (int vi = 0; vi < 3; vi++) {
      tri.vertices[vi].x = mesh.vertices[3 * (i + vi)];
      tri.vertices[vi].y = mesh.vertices[3 * (i + vi) + 1];
      tri.vertices[vi].z = mesh.vertices[3 * (i + vi) + 2];
    }

    particles.triangles.push_back(tri);
  }

  return particles;
}

void Particles::Draw(Vector3 pos, float time) {
  float speed = 10;
  rlPushMatrix();
  rlTranslatef(pos.x, pos.y, pos.z);
  for (auto tri : triangles) {
    rlPushMatrix();
    rlTranslatef(tri.dir.x * speed * time, tri.dir.z * speed * time, tri.dir.y * speed * time);
    DrawLineTriangle3DCull(tri.vertices[0], tri.vertices[1], tri.vertices[2], RED);
    rlPopMatrix();
  }
  rlPopMatrix();
}