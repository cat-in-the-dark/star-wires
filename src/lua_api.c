#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <raylib.h>
#include <rlgl.h>


extern Model models[128];
static lua_State* L;


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

void CallFunc(const char* name) {
  lua_getglobal(L, name);
  if (lua_isfunction(L, -1)) {
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
      const char* msg = lua_tostring(L, -1);
      TraceLog(LOG_ERROR, "lua: %s", msg);
    }
  } else {
    TraceLog(LOG_ERROR, "lua: %s is not a function", name);
  }
}

int lua_DrawModel(lua_State* L) {
  int n = lua_gettop(L);
  if (n == 5) {
    int model_id = luaL_checknumber(L, 1) - 1;
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);
    float scale = luaL_checknumber(L, 5);

    Vector3 pos = {x, y, z};
    Model model = models[model_id];  // TODO: check model_id out of bound

    DrawLineModel(model, pos, scale, RED);
    DrawModel(model, pos, scale * 0.99, BLACK);  // FIXME: the bigger scale the bigger scale offset. not good
  } else {
    luaL_error(L, "invalid parameters, mdl(idx, x,y,z, scale)");
  }

  return 0;
}

int lua_DrawModelEx(lua_State* L) {
  int n = lua_gettop(L);
  if (n == 9) {
    int model_id = luaL_checknumber(L, 1) - 1;
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);
    float scale = luaL_checknumber(L, 5);

    float x_axis = luaL_checknumber(L, 6);
    float y_axis = luaL_checknumber(L, 7);
    float z_axis = luaL_checknumber(L, 8);
    float angle = luaL_checknumber(L, 9);

    Vector3 pos = {x, y, z};
    Vector3 axis = {x_axis, y_axis, z_axis};
    Vector3 vscale = {scale, scale, scale};
    Model model = models[model_id];  // TODO: check model_id out of bound

    DrawLineModelEx(model, pos, axis, angle, scale, RED);
    DrawModelEx(model, pos, axis, angle, vscale, BLACK);  // FIXME: the bigger scale the bigger scale offset. not good
  } else {
    luaL_error(L, "invalid parameters, mdl(idx, x,y,z, scale)");
  }

  return 0;
}

void lua_Update(void) {
  lua_pushnumber(L, GetFrameTime());
  lua_setglobal(L, "dt");

  lua_pushnumber(L, GetTime());
  lua_setglobal(L, "time");

  CallFunc("Update");
}


int lua_Init(void) {
  L = luaL_newstate();
  luaL_openlibs(L);

  lua_pushnumber(L, 0);
  lua_setglobal(L, "dt");

  lua_pushnumber(L, 0);
  lua_setglobal(L, "time");

  lua_pushcfunction(L, lua_DrawModel);
  lua_setglobal(L, "mdl");

  lua_pushcfunction(L, lua_DrawModelEx);
  lua_setglobal(L, "mdlex");

  int err = luaL_dofile(L, "assets/main.lua");
  if (err != LUA_OK) {
    const char* msg = lua_tostring(L, -1);
    TraceLog(LOG_ERROR, "lua: %s", msg);
    return -1;
  }

  TraceLog(LOG_INFO, "Lua loaded\n");

  CallFunc("Init");
  return 0;
}

void lua_Draw(void) {
  CallFunc("Draw");
}

void lua_Dispose() {
  lua_close(L);
}
