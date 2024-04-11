#include "AssetManager.hpp"

void AssetManager::Init() {
  shader2id[NO_SHADE] = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
  shader2id[TERRAIN] = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
  shader2id[LAMP] = loadShaders("shaders/lamp.vert", "shaders/lamp.frag");

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

  glActiveTexture(GL_TEXTURE5);
  LoadTGATextureSimple("textures/white.tga", &white);
  tex2id[WHITE] = 5;

  glActiveTexture(GL_TEXTURE6);
  LoadTGATextureSimple("textures/offwhite.tga", &offwhite);
  tex2id[OFFWHITE] = 6;

  glActiveTexture(GL_TEXTURE7);
  LoadTGATextureSimple("textures/black.tga", &black);
  tex2id[BLACK] = 7;

  glActiveTexture(GL_TEXTURE8);
  LoadTGATextureSimple("textures/carton.tga", &carton);
  tex2id[CARTON_TEX] = 8;

  modelType2Model[SKYBOX] = LoadModelPlus("objects/skybox.obj");
  modelType2Model[SPHERE] = LoadModelPlus("objects/groundsphere.obj");
  modelType2Model[CHAIR] = LoadModelPlus("objects/chair.obj");
  modelType2Model[CARTON] = LoadModelPlus("objects/carton.obj");
}

GLuint AssetManager::getShaderId(ShaderType shader) { return shader2id[shader]; }

int AssetManager::getTexId(TextureType tex) { return tex2id[tex]; }

Model* AssetManager::getModel(ModelType model) { return modelType2Model[model]; }