#pragma once

#include "GL_utilities.h"
#include "LoadTGA.h"
#include "core/Enums.hpp"
#include "map"
#include "string"

class ShaderManager {
 public:
  void Init();

  GLuint getShaderId(ShaderType shader);
  int getTexId(TextureType tex);

 private:
  std::map<ShaderType, GLuint> shader2id;
  std::map<TextureType, int> tex2id;
  GLuint officeWall, grass, skyboxSky, officeFloor, officeCeiling;
};