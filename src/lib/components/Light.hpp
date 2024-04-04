#pragma once

#include "VectorUtils4.h"
#include "core/Enums.hpp"

struct Light {
  GLint isDirectional = 0;
  vec3 color;
  ShaderType shader;
};