#include "AssetManager.hpp"

void AssetManager::Init() {
  shader2id[NO_SHADE] = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
  shader2id[TERRAIN] = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
  shader2id[LAMP] = loadShaders("shaders/lamp.vert", "shaders/lamp.frag");
  shader2id[ADD] = loadShaders("shaders/add.vert", "shaders/add.frag");
  shader2id[LOWPASS_X] = loadShaders("shaders/lowpass.vert", "shaders/lowpassX.frag");
  shader2id[LOWPASS_Y] = loadShaders("shaders/lowpass.vert", "shaders/lowpassY.frag");
  shader2id[THRES] = loadShaders("shaders/threshold.vert", "shaders/threshold.frag");
  shader2id[PLAIN] = loadShaders("shaders/plain.vert", "shaders/plain.frag");

  glActiveTexture(GL_TEXTURE9);
  LoadTGATextureSimple("textures/floor.tga", &officeFloor);
  tex2id[OFFICE_FLOOR] = 9;

  glActiveTexture(GL_TEXTURE10);
  LoadTGATextureSimple("textures/wall.tga", &officeWall);
  tex2id[OFFICE_WALL] = 10;

  glActiveTexture(GL_TEXTURE2);
  LoadTGATextureSimple("textures/skybox3.tga", &skyboxSky);
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

  // MODELS

  GLfloat square[] = {-1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0};
  GLfloat squareTexCoord[] = {0, 0, 0, 1, 1, 1, 1, 0};
  GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

  modelType2Model[SKYBOX] = LoadModelPlus("objects/skybox3.obj");
  modelType2Model[SPHERE] = LoadModelPlus("objects/groundsphere.obj");
  modelType2Model[CHAIR] = LoadModelPlus("objects/chair.obj");
  modelType2Model[CARTON] = LoadModelPlus("objects/carton.obj");
  modelType2Model[SQUARE] = LoadDataToModel((vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL, squareIndices, 4, 6);

  // FBO
  const int WINDOW_WIDTH = 600;
  const int WINDOW_HEIGHT = 600;
  fbo1 = initFBO(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  fbo2 = initFBO(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  fbo3 = initFBO(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  fbo4 = initFBO(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
}

GLuint AssetManager::getShaderId(ShaderType shader) { return shader2id[shader]; }

int AssetManager::getTexId(TextureType tex) { return tex2id[tex]; }

Model *AssetManager::getModel(ModelType model) { return modelType2Model[model]; }

FBOstruct *AssetManager::getFBO1() { return fbo1; }
FBOstruct *AssetManager::getFBO2() { return fbo2; }
FBOstruct *AssetManager::getFBO3() { return fbo3; }
FBOstruct *AssetManager::getFBO4() { return fbo4; }