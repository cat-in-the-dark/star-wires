#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>

#include "lapi.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static LApi lApi;

Model models[128];

Camera camera = {0};

static RenderTexture2D target;
static Shader bloom;
static Shader barycentricShader;
static int barycentricLoc;

void Draw(void) {
  // TODO: c/c++ render
}

void Update() {
  lApi.Update();

  BeginTextureMode(target);

  ClearBackground(BLACK);

  BeginMode3D(camera);

  // Draw();

  lApi.Draw();

  EndMode3D();

  lApi.DrawCanvas();

  EndTextureMode();

  BeginDrawing();
  ClearBackground(BLACK);

  BeginShaderMode(bloom);
  DrawTextureRec(target.texture, (Rectangle){0, 0, (float) target.texture.width, (float) -target.texture.height},
                 (Vector2){0, 0}, WHITE);
  EndShaderMode();

  DrawFPS(10, 10);

  EndDrawing();
}

int calculateBarycentric(float** barycentric, int n_vertices) {
  // For every triangle consisting of 6 vertices (3 points of two coordinates)
  int n = n_vertices / 3;

  // we will have three points of three coordinates, respectively:
  // p_1 = (1, 0, 0), p_2 = (0, 1, 0), p_3 = (0, 0, 1).
  float coords[6] = {
      1, 0, 0, 1, 0, 0,
  };

  TraceLog(LOG_INFO, "calculateBarycentric n=%d vertices=%d", n, n_vertices);
  float* bc = reinterpret_cast<float*>(MemAlloc(6 * n * sizeof(float)));
  if (!bc) {
    TraceLog(LOG_ERROR, "failed to allocate barycentric coordinates");
    return -1;
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 6; j++) {
      bc[i * 6 + j] = coords[j];
    }
  }

  *barycentric = bc;
  return n * 6;
}

void applyWireframesMaterial(Model model) {
  float* barycentric;
  int n_barycentric = calculateBarycentric(&barycentric, model.meshes[0].vertexCount);

  rlEnableVertexArray(model.meshes[0].vaoId);
  rlLoadVertexBuffer(barycentric, n_barycentric * sizeof(float), false);
  rlEnableVertexAttribute(barycentricLoc);
  rlSetVertexAttribute(barycentricLoc, 2, RL_FLOAT, false, 0, (void*) 0);
  rlDisableVertexArray();

  model.materials[0].shader = barycentricShader;

  MemFree(barycentric);
}

typedef struct LineMesh {
  int vertexCount;  // Number of vertices stored in arrays
  int linesCount;   // Number of lines stored

  float* vertices;  // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
  int* lines;

  // TODO: animation data?

  // OpenGL identifiers
  unsigned int vaoId;  // OpenGL Vertex Array Object id
  unsigned int vboId;  // OpenGL Vertex Buffer Objects id (default vertex data)
} LineMesh;

typedef struct LineModel {
  Matrix transform;  // Local transform matrix

  LineMesh* meshes;

  // TODO: animation data?
} LineModel;

void loadModels() {
  int i = 0;
  models[i++] = LoadModel("assets/rock_01.obj");
  models[i++] = LoadModel("assets/rock_02.obj");
  models[i++] = LoadModel("assets/rock_03.obj");
  models[i++] = LoadModel("assets/rock_04.obj");
  models[i++] = LoadModel("assets/rock_05.obj");
  models[i++] = LoadModel("assets/rock_13.obj");
  models[i++] = LoadModel("assets/rock_20.obj");
  models[i++] = LoadModel("assets/rock_21.obj");
}

int main(void) {
  const int screenWidth = 640;
  const int screenHeight = 480;

  InitWindow(screenWidth, screenHeight, "raylib [3d]");
  // ToggleFullscreen();

  DisableCursor();  // Limit cursor to relative movement inside the window

  // char *extensions = (char *)glGetString(GL_EXTENSIONS);

  // Define the camera to look into our 3d world
  camera.position = {0.0f, 0.0f, -10.0f};  // Camera position
  camera.target = {0.0f, 0.0f, 1.0f};      // Camera looking at point
  camera.up = {0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
  camera.fovy = 45.0f;                     // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  loadModels();

  bloom = LoadShader(0, TextFormat("assets/shaders/glsl%i/bloom.fs", GLSL_VERSION));

  target = LoadRenderTexture(screenWidth, screenHeight);

  SetTargetFPS(60);

  lApi.Run();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(Update, 0, 1);
#else
  while (!WindowShouldClose()) {
    Update();
  }
#endif

  UnloadModel(models[0]);  // Unload model
  UnloadRenderTexture(target);
  UnloadShader(bloom);
  UnloadShader(barycentricShader);

  CloseWindow();  // Close window and OpenGL context

  return 0;
}