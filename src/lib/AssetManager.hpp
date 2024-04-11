#pragma once

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "core/Enums.hpp"
#include "map"
#include "string"

class AssetManager {
 public:
  void Init();

  GLuint getShaderId(ShaderType shader);
  int getTexId(TextureType tex);
  Model *getModel(ModelType model);

 private:
  std::map<ShaderType, GLuint> shader2id;
  std::map<TextureType, int> tex2id;
  std::map<ModelType, Model *> modelType2Model;
  GLuint officeWall, grass, skyboxSky, officeFloor, officeCeiling, white, offwhite;
};