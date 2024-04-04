#include "RenderSystem.hpp"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "boxes.h"
#include "camera.hpp"
#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "ground.h"

extern Coordinator gCoordinator;

void RenderSystem::drawSkybox() {
  glUseProgram(noShadeProgram);
  // disable depth test and culling
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // camera matrix without translation
  mat4 skyBoxTransform = cameraMatrix;

  // remove translation
  skyBoxTransform.m[3] = 0;
  skyBoxTransform.m[7] = 0;
  skyBoxTransform.m[11] = 0;

  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "cameraMatrix"), 1, GL_TRUE, skyBoxTransform.m);

  mat4 trans = T(0.0f, -0.3f, 0.0f);
  mat4 rot = Ry(0);
  mat4 total = rot * trans;
  mat4 scale = S(20.0f, 20.0f, 20.0f);
  total = total * scale;
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
  // set texture as skybox texture (texUnit = 2)
  glUniform1i(glGetUniformLocation(noShadeProgram, "texUnit"), 2);
  DrawModel(skyboxModel, noShadeProgram, "inPosition", NULL, "inTexCoord");

  // enable depth test and culling
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void RenderSystem::Init() {
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  printError("GL inits");

  printf("The entity id is %d\n", mCamera);

  float far = 1000.0f;
  float near = 0.2f;
  mat4 projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, near, far);

  // Add camera and transform component to entity
  mCamera = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(mCamera, Transform{.position = vec3(60.0f, 10.0f, 0.0f)});
  gCoordinator.AddComponent(mCamera, Camera{.projectionTransform = projectionMatrix,
                                            .theta = -2 * M_PI_2,
                                            .phi = 0,
                                            .lookAt = vec3(0.0f, 10.0f, 0.0f),
                                            .cameraUp = vec3(0.0f, 1.0f, 0.0f)});

  // init shaders
  noShadeProgram = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
  printError("init shader");

  // Upload projection matrix to shader
  glUseProgram(noShadeProgram);
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  // end projection upload

  // Load textures from file
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("textures/floor.tga", &backroomsFloorTex);
  glActiveTexture(GL_TEXTURE1);
  LoadTGATextureSimple("textures/wall.tga", &backroomsWallTex);
  glActiveTexture(GL_TEXTURE2);
  LoadTGATextureSimple("textures/skybox.tga", &skyboxTex);
  glActiveTexture(GL_TEXTURE3);
  LoadTGATextureSimple("textures/grass.tga", &grassTex);
  // end load textures

  // load models
  skyboxModel = LoadModelPlus("objects/skybox.obj");

  printError("init");
}

void RenderSystem::Update() {
  drawSkybox();
  for (auto &entity : mEntities) {
    Model *model = gCoordinator.GetComponent<Renderable>(entity).model;
    mat4 trans = gCoordinator.GetComponent<Transform>(entity).translation;
    mat4 rot = gCoordinator.GetComponent<Transform>(entity).rotation;
    GLuint program = gCoordinator.GetComponent<Renderable>(entity).program;
    GLuint texUnit = gCoordinator.GetComponent<Renderable>(entity).texUnit;
    glUseProgram(program);
    mat4 m2w = trans * rot;
    mat4 total = cameraMatrix * m2w;
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "model2World"), 1, GL_TRUE, m2w.m);
    glUniform1i(glGetUniformLocation(program, "texUnit"), texUnit);
    DrawModel(model, program, "inPosition", "inNormal", "inTexCoord");
  }

  // Update camera matrix
  auto &camera = gCoordinator.GetComponent<Camera>(mCamera);
  cameraMatrix = camera.matrix;
}