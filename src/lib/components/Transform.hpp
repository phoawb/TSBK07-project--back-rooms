#pragma once

#include "VectorUtils4.h"

struct Transform {
  mat4 translation = IdentityMatrix();
  mat4 rotation = IdentityMatrix();
  mat4 scale = IdentityMatrix();
};