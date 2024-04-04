#pragma once

#include "LittleOBJLoader.h"
#include "core/Enums.hpp"

struct Renderable {
  Model* model;
  ShaderType shader;
  GLuint texUnit;
};