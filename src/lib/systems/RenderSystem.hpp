#pragma once

#include "LightManager.hpp"
#include "LittleOBJLoader.h"
#include "core/System.hpp"

class Event;

class RenderSystem : public System {
 public:
  int getLightCount(const LightManager &lightManager);

  void placeLight(vec3 lightPos, vec3 lightColor, bool isDirectional);

  void drawSkybox();

  void drawGroundSphere();

  void drawGround();

  void Init();

  void Update();

 private:
  Entity mCamera;
  mat4 cameraMatrix;
  Model *groundSphereModel, *skyboxModel, *groundModel;
  GLuint terrainProgram, noShadeProgram;
  GLuint backroomsWallTex, backroomsFloorTex, skyboxTex, grassTex;
  vec3 groundBallPos = vec3(0, 0, 0);

  LightManager lightManager;
};
