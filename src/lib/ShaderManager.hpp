#pragma once

#include "GL_utilities.h"
#include "core/Enums.hpp"
#include "map"
#include "string"

class ShaderManager {
 public:
  void Init() {
    shader2id[NO_SHADE] = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
    shader2id[TERRAIN] = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
  }
  GLuint getShaderId(ShaderType shader) { return shader2id[shader]; }

 private:
  std::map<ShaderType, GLuint> shader2id;
};