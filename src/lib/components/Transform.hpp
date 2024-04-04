#pragma once

#include "VectorUtils4.h"

struct Transform {
  vec3 position;
  mat4 translation;
  mat4 rotation;
  vec3 scale;
};