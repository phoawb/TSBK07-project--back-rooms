#pragma once

#include "VectorUtils4.h"
#include "core/System.hpp"
class Event;

class CameraControlSystem : public System {
 public:
  void Init();

  void Update(float dt);

 private:
  //mat4 cameraMatrix;
  vec3 cameraPos = {60.f, 10.f, 0.f};

  void InputListener(Event& event);
};
