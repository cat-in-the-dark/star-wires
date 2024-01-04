#pragma once

#include <raylib.h>
#include <rlgl.h>

void DrawLineModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, float scale, Color color) {
  float x = 0;
  float y = 0;
  float z = 0;

  Mesh mesh = model.meshes[0];

  rlPushMatrix();
  rlTranslatef(position.x, position.y, position.z);
  rlRotatef(rotationAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
  rlScalef(scale, scale, scale);

  rlEnableBackfaceCulling();
  rlBegin(RL_LINES);
  rlColor4ub(color.r, color.g, color.b, color.a);

  // TODO: maybe we can load vertices for line in a buffer and drender directly from gpu mem?
  for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
    // unsigned short index1 = mesh.indices[i];
    // unsigned short index2 = mesh.indices[i + 1];
    // unsigned short index3 = mesh.indices[i + 2];
    int index1 = i;
    int index2 = i + 1;
    int index3 = i + 2;

    // Draw lines for each edge of the triangle
    rlVertex3f(mesh.vertices[3 * index1], mesh.vertices[3 * index1 + 1], mesh.vertices[3 * index1 + 2]);
    rlVertex3f(mesh.vertices[3 * index2], mesh.vertices[3 * index2 + 1], mesh.vertices[3 * index2 + 2]);

    rlVertex3f(mesh.vertices[3 * index2], mesh.vertices[3 * index2 + 1], mesh.vertices[3 * index2 + 2]);
    rlVertex3f(mesh.vertices[3 * index3], mesh.vertices[3 * index3 + 1], mesh.vertices[3 * index3 + 2]);

    rlVertex3f(mesh.vertices[3 * index3], mesh.vertices[3 * index3 + 1], mesh.vertices[3 * index3 + 2]);
    rlVertex3f(mesh.vertices[3 * index1], mesh.vertices[3 * index1 + 1], mesh.vertices[3 * index1 + 2]);
  }

  rlDisableBackfaceCulling();
  rlEnd();
  rlPopMatrix();
}

void DrawLineModel(Model model, Vector3 position, float scale, Color color) {
  Vector3 rotationAxis = {0, 0, 0};
  DrawLineModelEx(model, position, rotationAxis, 0.0f, scale, color);
}

void DrawLineModelCulled(Model model, Vector3 position, float scale) {
  DrawLineModel(model, position, scale, RED);
  DrawModel(model, position, scale * 0.99, BLACK);  // FIXME: the bigger scale the bigger scale offset. not good
}

void DrawLineModelCulledEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, float scale) {
  DrawLineModelEx(model, position, rotationAxis, rotationAngle, scale, RED);
  DrawModelEx(model, position, rotationAxis, rotationAngle, {scale, scale, scale}, BLACK);  // FIXME: the bigger scale the bigger scale offset. not good
}