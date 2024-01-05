#include "lapi.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include "particles.h"
#include "renderer.h"
#include "shaker.h"

extern Model models[128];
extern Particles particles[128];
extern Camera camera;
extern float sizes[128];

float NormedMouseX() {
  float mx = GetMouseX();
  float sw = GetScreenWidth();
  return (mx / sw - 0.5) * 2.0;
}

float NormedMouseY() {
  float my = GetMouseY();
  float sh = GetScreenHeight();
  return (0.5 - my / sh) * 2.0;
}

void must(sol::protected_function_result result) {
  if (!result.valid()) {
    sol::error err = result;
    TraceLog(LOG_ERROR, err.what());
    exit(-1);
  }
}

void lua_DrawModelEx(int model_id, float x, float y, float z, float scale, float x_axis, float y_axis, float z_axis,
                     float angle) {
  Model model = models[model_id - 1];  // TODO: check model_id out of bound
  DrawLineModelCulledEx(model, {x, y, z}, {x_axis, y_axis, z_axis}, angle, scale);
}

void lua_DrawModel(int model_id, float x, float y, float z, float scale) {
  Model model = models[model_id - 1];  // TODO: check model_id out of bound
  DrawLineModelCulled(model, {x, y, z}, scale);
}

void lua_DrawParticles(int particle_id, float x, float y, float z, float x_axis, float y_axis, float z_axis,
                       float angle, float time, float speed, float lifetime) {
  Particles p = particles[particle_id - 1];
  p.Draw({x, y, z}, angle, {x_axis, y_axis, z_axis}, time, speed, lifetime);
}

auto lua_ModelLineCollision(sol::state& lua, int i, float px, float py, float pz, float rx, float ry, float rz,
                            float angle, float scale, float lx1, float ly1, float lz1, float lx2, float ly2,
                            float lz2) {
  Vector3 lineStart = {lx1, ly1, lz1};
  Vector3 lineEnd = {lx2, ly2, lz2};
  auto lineLength = 2.0f;
  Ray ray = {lineStart, Vector3Subtract(lineEnd, lineStart)};
  auto model = models[i];

  Vector3 position = {px, py, pz};
  Vector3 rotationAxis = {rx, ry, rz};

  auto distance = Vector3Distance(position, lineEnd);
  if (distance < lineLength) {
    return lua.create_table_with("hit", true, "dist", distance);
  }
  // } else {
  //   TraceLog(LOG_INFO, "Close encounter: %.3f %.3f %.3f %.3f <=> %.3f %.3f %.3f", distance, px, py, pz, lx2, ly2,
  //   lz2);
  // }

  // Calculate transformation matrix from function parameters
  // Get transform matrix (rotation -> scale -> translation)
  Matrix matScale = MatrixScale(scale, scale, scale);
  Matrix matRotation = MatrixRotate(rotationAxis, angle * DEG2RAD);
  Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);
  Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

  auto result = GetRayCollisionMesh(ray, model.meshes[0], matTransform);
  if (result.hit) {
    TraceLog(LOG_INFO, "Ray hit %.3f %.3f %.3f <=> %.3f %.3f %.3f", px, py, pz, lx2, ly2, lz2);
    if (result.distance < lineLength) {
      TraceLog(LOG_INFO, "Line hit %.3f %.3f %.3f %.3f <=> %.3f %.3f %.3f", result.distance, px, py, pz, lx2, ly2, lz2);
      return lua.create_table_with("hit", true, "dist", result.distance);
    }
  }

  return lua.create_table_with("hit", false, "dist", result.distance);
};

LApi::LApi() {
  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::base, sol::lib::string, sol::lib::math,
                     sol::lib::table, sol::lib::coroutine);
}

void LApi::Draw() {
  must(lua["Draw"].call());
}

void LApi::DrawCanvas() {
  must(lua["DrawCanvas"].call());
}

void LApi::Update() {
  lua["dt"] = GetFrameTime();
  lua["time"] = GetTime();

  lua["mouse"]["px"] = GetMouseX();
  lua["mouse"]["py"] = GetMouseY();

  auto mouseDelta = GetMouseDelta();
  lua["mouse"]["dx"] = mouseDelta.x;
  lua["mouse"]["dy"] = mouseDelta.y;

  must(lua["Update"].call());

  lua["camera"]["px"] = camera.position.x;
  lua["camera"]["py"] = camera.position.y;
  lua["camera"]["pz"] = camera.position.z;
  lua["camera"]["tx"] = camera.target.x;
  lua["camera"]["ty"] = camera.target.y;
  lua["camera"]["tz"] = camera.target.z;
}

void LApi::Run() {
  lua["dt"] = 0.0;
  lua["time"] = 0.0;
  lua["mdl"] = lua_DrawModel;
  lua["mdlex"] = lua_DrawModelEx;
  lua["ptcl"] = lua_DrawParticles;
  lua["circle"] = [](float x, float y, float radius) { DrawCircleV({x, y}, radius, RED); };
  lua["camera"] = lua.create_table_with(
      "px", camera.position.x, "py", camera.position.y, "pz", camera.position.z, "tx", camera.target.x, "ty",
      camera.target.y, "tz", camera.target.z, "moveUp", [](float distance) { CameraMoveUp(&camera, distance); },
      "moveRight", [](float distance) { CameraMoveRight(&camera, distance, true); }, "setPos",
      [](float x, float y, float z) {
        camera.position.x = x;
        camera.position.y = y;
        camera.position.z = z;
      },
      "setTarget",
      [](float x, float y, float z) {
        camera.target.x = x;
        camera.target.y = y;
        camera.target.z = z;
      },
      "shake", StartCameraShaker);
  lua["mouse"] = lua.create_table_with("px", GetMouseX(), "py", GetMouseY(), "dx", 0, "dy", 0);
  lua["screen"] = lua.create_table_with("width", GetScreenWidth(), "height", GetScreenHeight());
  lua["star"] = [](float x, float y, float z, float radius) { DrawStar({x, y, z}, radius); };
  lua["ray"] = [&](float x, float y) {
    auto res = GetMouseRay({x, y}, camera);
    auto pos = res.position;
    auto dir = res.direction;

    return lua.create_table_with("pos", lua.create_table_with("x", pos.x, "y", pos.y, "z", pos.z), "dir",
                                 lua.create_table_with("x", dir.x, "y", dir.y, "z", dir.z));
  };
  lua["ismouse"] = []() { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT); };
  lua["line3d"] = [](float x1, float y1, float z1, float x2, float y2, float z2) {
    DrawLine3D({x1, y1, z1}, {x2, y2, z2}, GREEN);
  };
  lua["rawray"] = [](float x, float y) { return GetMouseRay({x, y}, camera); };
  lua["drawray"] = [](Ray ray) {
    auto pos = ray.position;
    auto dir = ray.direction;
    TraceLog(LOG_INFO, "%.3f %.3f %.3f => %.3f %.3f %.3f", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
    DrawRay(ray, GREEN);
  };
  lua["collmdlline"] = [&lua = lua](int i, float px, float py, float pz, float rx, float ry, float rz, float angle,
                                    float scale, float lx1, float ly1, float lz1, float lx2, float ly2, float lz2) {
    return lua_ModelLineCollision(lua, i, px, py, pz, rx, ry, rz, angle, scale, lx1, ly1, lz1, lx2, ly2, lz2);
  };

  must(lua.script_file("assets/main.lua"));

  TraceLog(LOG_INFO, "Lua loaded\n");

  must(lua["Init"].call());
}
