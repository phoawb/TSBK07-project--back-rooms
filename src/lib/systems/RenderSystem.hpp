#pragma once

#include "ecs.hpp"
#include "memory"

class Event;

class RenderSystem : public System {
 public:
  int getLightCount(const LightManager &lightManager);

  void placeLight(vec3 lightPos, vec3 lightColor, bool isDirectional);

  void drawSkybox();

  void drawGroundSphere();

  void drawGround();

  void drawObjects();

  void Init();

  void Update();

 private:
  Entity mCamera;
  mat4 projectionMatrix;
  float theta = 0;
  float phi = 0;
  mat4 cameraMatrix;
  Model *groundSphereModel, *skyboxModel, *groundModel;
  // Reference to shader program
  GLuint terrainProgram, noShadeProgram;
  // Reference to textures
  GLuint backroomsWallTex, backroomsFloorTex, skyboxTex, grassTex;
  vec3 groundBallPos = vec3(0, 0, 0);

  LightManager lightManager;

  // GLuint mVao{};
  // GLuint mVboVertices{};
  // GLuint mVboNormals{};
};
