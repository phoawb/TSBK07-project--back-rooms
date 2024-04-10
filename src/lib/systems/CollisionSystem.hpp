#pragma once

#include "VectorUtils4.h"
#include "core/System.hpp"

class CollisionSystem : public System {
 public:
  void Init();

  void TranslationAdd(mat4& a, vec3 b);

  void Update();
};