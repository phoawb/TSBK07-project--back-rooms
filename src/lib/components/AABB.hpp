#pragma once

#include "VectorUtils4.h"

struct AABB {
  vec3 dimensions;
  bool centered = false;
};