#pragma once

#include "VectorUtils4.h"
#include "core/System.hpp"

class CollisionSystem : public System {
 public:
  void Init();

  bool collissionPointAABB(vec3 pos1, vec3 min2, vec3 max2);

  bool collisionAABB(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

  void Update();
};