#pragma once

#include "LightManager.hpp"
#include "LittleOBJLoader.h"
#include "core/System.hpp"

class Event;

class RenderSystem : public System {
 public:
  void drawSkybox();

  void Init();

  void Update();

 private:
  Entity mCamera;
  mat4 cameraMatrix;
  mat4 projectionMatrix;
  LightManager lightManager;
  Model *groundSphereModel, *skyboxModel, *groundModel;
  GLuint terrainProgram, noShadeProgram;
  GLuint backroomsWallTex, backroomsFloorTex, skyboxTex, grassTex;
  vec3 groundBallPos = vec3(0, 0, 0);
};
