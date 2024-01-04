#pragma once

#include <raylib.h>

void DrawTriangle3DWires(Vector3 v1, Vector3 v2, Vector3 v3, Color color);

void DrawLineTriangle3DCull(Vector3 v1, Vector3 v2, Vector3 v3, Color color);

void DrawStar(Vector3 position, float radius);

void DrawLineModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, float scale,
                     Color color);

void DrawLineModel(Model model, Vector3 position, float scale, Color color);

void DrawLineModelCulled(Model model, Vector3 position, float scale);

void DrawLineModelCulledEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, float scale);