#pragma once

#include "VectorUtils4.h"

struct Transform {
  mat4 translation = T(0, 0, 0);
  mat4 rotation = Ry(0);
  vec3 scale = vec3(1.f);
};