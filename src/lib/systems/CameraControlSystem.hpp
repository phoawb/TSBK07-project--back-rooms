#pragma once

#include "VectorUtils4.h"
#include "core/System.hpp"
class Event;

class CameraControlSystem : public System {
 public:
  void Init();

  void Update(int mouseX, int mouseY);

 private:
  void InputListener(Event& event);
};
