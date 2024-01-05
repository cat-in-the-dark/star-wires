#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>

#include "lapi.h"
#include "mymath.h"
#include "particles.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include "shaker.h"

static LApi* lApi;

Particles particles[128];
float sizes[128];
Model models[128];
int n_models = 0;

Camera debugCamera = {0};
Camera camera = {0};

bool debug = false;

RenderTexture2D target;
static RenderTexture2D debugTarget;
static Shader bloom;
Shader blur;
static int barycentricLoc;

void Draw(void) {
  // TODO: c/c++ render
}

void Update() {
  lApi->Update();

  if (IsKeyDown(KEY_UP)) {
    debugCamera.position.y += 1;
  }
  if (IsKeyDown(KEY_DOWN)) {
    debugCamera.position.y -= 1;
  }
  if (IsKeyDown(KEY_LEFT)) {
    debugCamera.position.x -= 1;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    debugCamera.position.x += 1;
  }
  if (IsKeyDown(KEY_W)) {
    debugCamera.target.y += 1;
  }
  if (IsKeyDown(KEY_S)) {
    debugCamera.target.y -= 1;
  }
  if (IsKeyDown(KEY_A)) {
    debugCamera.target.x -= 1;
  }
  if (IsKeyDown(KEY_D)) {
    debugCamera.target.x += 1;
  }
  if (IsKeyDown(KEY_Q)) {
    debugCamera.position.z += 1;
  }
  if (IsKeyDown(KEY_E)) {
    debugCamera.position.z -= 1;
  }
  if (IsKeyPressed(KEY_O)) {
    debug = !debug;
  }
  if (IsKeyDown(KEY_SPACE)) {
    StartCameraShaker();
  }

  // UpdateCamera(&camera, CAMERA_ORBITAL);

  BeginTextureMode(target);

  ClearBackground(BLACK);

  BeginMode3D(camera);

  // Draw();

  lApi->Draw();

  EndMode3D();

  lApi->DrawCanvas();

  EndTextureMode();

  BeginDrawing();
  ClearBackground(BLACK);

  BeginShaderMode(bloom);

  BeginShakeCamera();

  DrawTextureRec(target.texture, (Rectangle){0, 0, (float) target.texture.width, (float) -target.texture.height},
                 (Vector2){0, 0}, WHITE);

  EndUpdateShakeCamera();

  EndShaderMode();

  if (debug) {
    BeginTextureMode(debugTarget);

    ClearBackground(BLACK);

    BeginMode3D(debugCamera);

    lApi->Draw();

    EndMode3D();

    EndTextureMode();

    DrawTextureEx(debugTarget.texture, {0, 0}, 0, 1.0f, WHITE);
    // DrawTextureEx(target.texture, {0, 0}, 0, 0.3f, WHITE);
  }

  DrawFPS(10, 10);

  EndDrawing();
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
  n_models = 0;
  models[n_models++] = LoadModel("assets/rock_01.obj");
  models[n_models++] = LoadModel("assets/rock_02.obj");
  models[n_models++] = LoadModel("assets/rock_03.obj");
  models[n_models++] = LoadModel("assets/rock_04.obj");
  models[n_models++] = LoadModel("assets/rock_05.obj");
  models[n_models++] = LoadModel("assets/rock_13.obj");
  models[n_models++] = LoadModel("assets/rock_20.obj");
  models[n_models++] = LoadModel("assets/rock_21.obj");
  models[n_models++] = LoadModel("assets/pushka.obj");

  models[n_models++] = LoadModel("assets/gameover.obj");
}

void loadParticles() {
  for (int i = 0; i < n_models; i++) {
    particles[i] = MeshTriangleSplit(models[i], true);
  }
}

void calculateSizes() {
  for (int i = 0; i < n_models; i++) {
    sizes[i] = CalculateMeanDistance(models[i]);
  }
}

void RunGame() {
#if defined(PLATFORM_WEB)
  lApi = new LApi();
  lApi->Run();
  emscripten_set_main_loop(Update, 0, 1);
  delete lApi;
#else
  while (!WindowShouldClose()) {
    bool restart = false;
    lApi = new LApi();
    lApi->Run();
    while (!WindowShouldClose() && !restart) {
      restart = IsKeyPressed(KEY_R);
      Update();
    }
    delete lApi;
  }
#endif
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

  debugCamera.position = {0.0f, 0.0f, -10.0f};  // Camera position
  debugCamera.target = {0.0f, 0.0f, 1.0f};      // Camera looking at point
  debugCamera.up = {0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
  debugCamera.fovy = 45.0f;                     // Camera field-of-view Y
  debugCamera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  loadModels();
  loadParticles();
  calculateSizes();

  bloom = LoadShader(0, TextFormat("assets/shaders/glsl%i/bloom.fs", GLSL_VERSION));
  blur = LoadShader(0, TextFormat("assets/shaders/glsl%i/blur.fs", GLSL_VERSION));

  target = LoadRenderTexture(screenWidth, screenHeight);
  debugTarget = LoadRenderTexture(screenWidth, screenHeight);

  SetTargetFPS(60);

  RunGame();

  UnloadModel(models[0]);  // Unload model
  UnloadRenderTexture(target);
  UnloadShader(bloom);
  UnloadShader(blur);

  CloseWindow();  // Close window and OpenGL context

  return 0;
}