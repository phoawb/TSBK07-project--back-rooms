#pragma once

#include "LittleOBJLoader.h"
#include "core/System.hpp"

class Event;

class RenderSystem : public System {
 public:
  void drawSkybox();

  bool isOutsideRenderDistance(mat4 trans);

  void Init();

  void Update();

 private:
  float renderDistance;
  Entity mCamera;
  mat4 cameraMatrix;
  mat4 projectionMatrix;
};
