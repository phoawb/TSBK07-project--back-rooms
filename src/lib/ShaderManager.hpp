#pragma once

#include "GL_utilities.h"
#include "LoadTGA.h"
#include "core/Enums.hpp"
#include "map"
#include "string"

class ShaderManager {
 public:
  void Init() {
    shader2id[NO_SHADE] = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
    shader2id[TERRAIN] = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");

    glActiveTexture(GL_TEXTURE0);
    LoadTGATextureSimple("textures/floor.tga", &officeFloor);
    tex2id[OFFICE_FLOOR] = 0;

    glActiveTexture(GL_TEXTURE1);
    LoadTGATextureSimple("textures/wall.tga", &officeWall);
    tex2id[OFFICE_WALL] = 1;

    glActiveTexture(GL_TEXTURE2);
    LoadTGATextureSimple("textures/skybox.tga", &skyboxSky);
    tex2id[SKYBOX_SKY] = 2;

    glActiveTexture(GL_TEXTURE3);
    LoadTGATextureSimple("textures/grass.tga", &grass);
    tex2id[GRASS] = 3;

    glActiveTexture(GL_TEXTURE4);
    LoadTGATextureSimple("textures/drop_ceiling.tga", &officeCeiling);
    tex2id[OFFICE_CEILING] = 4;
  }
  GLuint getShaderId(ShaderType shader) { return shader2id[shader]; }
  int getTexId(TextureType tex) { return tex2id[tex]; }

 private:
  std::map<ShaderType, GLuint> shader2id;
  std::map<TextureType, int> tex2id;
  GLuint officeWall, grass, skyboxSky, skyboxCity, dirt, officeFloor, concrete, officeCeiling;
};