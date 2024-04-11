#pragma once

#include "LittleOBJLoader.h"
#include "core/Enums.hpp"

struct Renderable {
  Model* model;
  float modelScale = 1.0f;
  ShaderType shader;
  TextureType texture;
};