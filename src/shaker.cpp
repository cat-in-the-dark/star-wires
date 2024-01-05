#include "shaker.h"

#include "mymath.h"
#include "rlgl.h"

extern RenderTexture2D target;
extern Shader blur;
extern Camera camera;

static float unShakeSpeed = 3.0;
static float startShakePower = 2.0;
static float shakePower = 0;
static float shakeMaxX = 20;
static float shakeMaxY = 20;
static float shakeMaxAngle = 15;  // degrees

void StartCameraShaker() {
  shakePower = startShakePower;
}

void BeginShakeCamera() {
  shakePower -= GetFrameTime() * unShakeSpeed;

  if (shakePower > 0.001) {
    rlPushMatrix();
    float sx = shakeMaxX * shakePower * GetRandomNegOneToOne();
    float sy = shakeMaxY * shakePower * GetRandomNegOneToOne();
    float sa = shakeMaxAngle * shakePower * GetRandomNegOneToOne();
    rlTranslatef(target.texture.width / 2, target.texture.height / 2, 0);
    rlRotatef(sa, 0, 0, -1);
    rlTranslatef(-target.texture.width / 2, -target.texture.height / 2, 0);
    rlTranslatef(sx, sy, 0);
    BeginShaderMode(blur);
  } else {
    shakePower = 0;
  }
}

void EndUpdateShakeCamera() {
  if (shakePower > 0.001) {
    rlPopMatrix();
    EndShaderMode();
  }
}