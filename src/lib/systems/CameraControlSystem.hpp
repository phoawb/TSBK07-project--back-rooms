#pragma once

#include "VectorUtils4.h"
#include "core/System.hpp"
class Event;

class CameraControlSystem : public System {
 public:
  void Init();

  void Update(float dt);

 private:
  void InputListener(Event& event);
};
