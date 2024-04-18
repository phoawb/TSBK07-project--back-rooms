#pragma once

#include "MicroGlut.h"
#include "core/System.hpp"

class Event;

class GuiSystem : public System {
 public:
  void Init();

  void Update();

 private:
  float lastTime;
  int nbFrames;
  float msPerFrame;
  float fps;
  float cameraX;
  float cameraY;
  float cameraZ;
  float theta;
  float phi;
};
