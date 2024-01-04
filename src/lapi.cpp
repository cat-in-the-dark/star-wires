#include "lapi.h"

#include <raylib.h>
#include "renderer.h"

extern Model models[128];

void lua_DrawModelEx(
  int model_id, float x, float y, float z, float scale, 
  float x_axis, float y_axis, float z_axis, float angle
) {
  Model model = models[model_id - 1];  // TODO: check model_id out of bound
  DrawLineModelCulledEx(model, {x, y, z}, {x_axis, y_axis, z_axis}, angle, scale);
}

void lua_DrawModel(
  int model_id, float x, float y, float z, float scale
) {
  Model model = models[model_id - 1];  // TODO: check model_id out of bound
  DrawLineModelCulled(model, {x, y, z}, scale);
}

LApi::LApi() {
  lua.open_libraries(sol::lib::base, sol::lib::package);
}

void LApi::Draw() {
  lua["Draw"].call();
}

void LApi::Update() {
  lua["dt"] = GetFrameTime();
  lua["time"] = GetTime();
  lua["Update"].call();
}

void LApi::Run() {
  lua["dt"] = 0.0;
  lua["time"] = 0.0;
  lua["mdl"] = lua_DrawModel;
  lua["mdlex"] = lua_DrawModelEx;

  auto result = lua.script_file("assets/main.lua");
  if (!result.valid()) {
    sol::error err = result;
    TraceLog(LOG_ERROR, err.what());
    return;
  }

  TraceLog(LOG_INFO, "Lua loaded\n");

  lua["Init"].call();
}
