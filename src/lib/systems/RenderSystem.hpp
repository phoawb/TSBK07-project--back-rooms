#pragma once

#include "LittleOBJLoader.h"
#include "core/System.hpp"

class Event;

class RenderSystem : public System {
 public:
  void drawSkybox();

  void drawCameraSphere(vec3 cameraPos, mat4 cameraMatrix);

  void Init();

  void Update();

 private:
  Entity mCamera;
  mat4 cameraMatrix;
  mat4 projectionMatrix;
  Model *groundSphereModel, *skyboxModel, *groundModel;
  GLuint terrainProgram, noShadeProgram;
  vec3 groundBallPos = vec3(0, 0, 0);
};
