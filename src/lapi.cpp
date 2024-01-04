#include "lapi.h"

#include <raylib.h>

#include "renderer.h"

extern Model models[128];
extern Camera camera;

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

  camera.position.x = lua["camera"]["px"];
  camera.position.y = lua["camera"]["py"];
  camera.position.z = lua["camera"]["pz"];
  camera.target.x = lua["camera"]["tx"];
  camera.target.y = lua["camera"]["ty"];
  camera.target.z = lua["camera"]["tz"];
}

void LApi::Run() {
  lua["dt"] = 0.0;
  lua["time"] = 0.0;
  lua["mdl"] = lua_DrawModel;
  lua["mdlex"] = lua_DrawModelEx;
  lua["circle"] = [](float x, float y, float radius) { DrawCircleV({x, y}, radius, RED); };
  lua["camera"] = lua.create_table_with("px", camera.position.x, "py", camera.position.y, "pz", camera.position.z, "tx",
                                        camera.target.x, "ty", camera.target.y, "tz", camera.target.z);
  lua["mouse"] = lua.create_table_with("px", GetMouseX(), "py", GetMouseY(), "dx", 0, "dy", 0);
  lua["screen"] = lua.create_table_with("width", GetScreenWidth(), "height", GetScreenHeight());
  lua["star"] = [](float x, float y, float z, float radius) { DrawStar({x, y, z}, radius); };

  must(lua.script_file("assets/main.lua"));

  TraceLog(LOG_INFO, "Lua loaded\n");

  must(lua["Init"].call());
}
