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
  FBOstruct *getFBO1();
  FBOstruct *getFBO2();
  FBOstruct *getFBO3();
  FBOstruct *getFBO4();

 private:
  std::map<ShaderType, GLuint> shader2id;
  std::map<TextureType, int> tex2id;
  std::map<ModelType, Model *> modelType2Model;
  FBOstruct *fbo1, *fbo2, *fbo3, *fbo4;
  GLuint officeWall, grass, skyboxSky, officeFloor, officeCeiling, white, offwhite, black, carton;
};