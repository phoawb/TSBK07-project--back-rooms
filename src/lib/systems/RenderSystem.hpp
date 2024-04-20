#pragma once

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "core/System.hpp"

class Event;

class RenderSystem : public System {
 public:
  void runfilter(GLuint shader, FBOstruct *in1, FBOstruct *in2, FBOstruct *out);

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
