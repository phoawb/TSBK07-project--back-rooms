#pragma once

#include "core/System.hpp"

class Event;

class GuiSystem : public System {
 public:
  void Init();

  void Update();

 private:
  float cameraX;
  float cameraY;
  float cameraZ;
  float theta;
  float phi;
};
