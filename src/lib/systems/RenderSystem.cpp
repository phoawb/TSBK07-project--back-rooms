#include "renderSystem.hpp"

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

int RenderSystem::getLightCount(const LightManager &lightManager) { return lightManager.lightSourcesColors.size(); }

void RenderSystem::placeLight(vec3 lightPos, vec3 lightColor, bool isDirectional = false) {
  lightManager.lightSourcesColors.push_back(lightColor);
  lightManager.lightSourcesDirectionsPositions.push_back(lightPos);
  GLint isDir = isDirectional ? 1 : 0;
  lightManager.isDirectional.push_back(isDir);
}

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
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  DrawModel(skyboxModel, noShadeProgram, "inPosition", NULL, "inTexCoord");

  // enable depth test and culling
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void RenderSystem::drawGround() {
  glUseProgram(terrainProgram);
  mat4 trans = T(0.f, 0.f, 0.f);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, cameraMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, trans.m);
  // set texture as floor texture (texUnit = 0)
  glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), 0);
  DrawModel(groundModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
}

void RenderSystem::drawGroundSphere() {
  glUseProgram(terrainProgram);
  float t = 0.0f;
  groundBallPos.x += (t - groundBallPos.x);
  groundBallPos.z += (t - groundBallPos.z);
  groundBallPos.y = 0;
  mat4 trans = T(groundBallPos.x, groundBallPos.y, groundBallPos.z);
  mat4 total = cameraMatrix * trans;
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, trans.m);

  // set texture as wall texture
  glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), 3);
  DrawModel(groundSphereModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
}

void RenderSystem::drawObjects() {}

void RenderSystem::Init() {
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);

  mCamera = gCoordinator.CreateEntity();
  printf("The entity id is %d\n", mCamera);

  gCoordinator.AddComponent(mCamera, Transform{.position = vec3(10.0f, 0.0f, 100.0f)});
  gCoordinator.AddComponent(mCamera, Camera{.projectionTransform = projectionMatrix,
                                            .cameraLookAt = vec3(0.0f, 10.0f, 0.0f),
                                            .cameraUp = vec3(0.0f, 1.0f, 0.0f)});

  // set up projection matrix and camera matrix
  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);
  vec3 angles = cameraDirection(theta, phi);

  // init shaders
  terrainProgram = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
  noShadeProgram = loadShaders("shaders/noShade.vert", "shaders/noShade.frag");
  printError("init shader");
  // end init shaders

  // Upload projection matrix to shader
  glUseProgram(terrainProgram);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUseProgram(noShadeProgram);
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  // end projection upload

  placeLight(vec3(0, 40, 25), vec3(1, 1, 1));

  // Upload light data to shader
  glUseProgram(terrainProgram);
  int lightCount = getLightCount(lightManager);
  glUniform1i(glGetUniformLocation(terrainProgram, "lightCount"), lightCount);
  glUniform3fv(glGetUniformLocation(terrainProgram, "lightSourcesColors"), lightCount,
               &lightManager.lightSourcesColors[0].x);
  glUniform3fv(glGetUniformLocation(terrainProgram, "lightSourcesDirPos"), lightCount,
               &lightManager.lightSourcesDirectionsPositions[0].x);
  glUniform1iv(glGetUniformLocation(terrainProgram, "isDirectional"), lightCount, &lightManager.isDirectional[0]);
  // end light upload

  printError("init light");

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
  groundSphereModel = LoadModelPlus("objects/groundsphere.obj");
  skyboxModel = LoadModelPlus("objects/skybox.obj");
  groundModel = getGroundModel(50.0);
  // begin generate boxes
  // wall 1
  auto wall1 = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(wall1, Transform{.translation = T(-50, 0.0, -50), .rotation = Ry(0)});
  gCoordinator.AddComponent(wall1,
                            Renderable{.model = getBoxModel(100, 50, 2), .program = terrainProgram, .texUnit = 1});
  // wall 2
  auto wall2 = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(wall2, Transform{.translation = T(-50, 0, 50), .rotation = Ry(M_PI / 2)});
  gCoordinator.AddComponent(wall2,
                            Renderable{.model = getBoxModel(100, 50, 2), .program = terrainProgram, .texUnit = 1});
  // wall 3
  auto wall3 = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(wall3, Transform{.translation = T(-50, 0, -50), .rotation = Ry(M_PI / 2)});
  gCoordinator.AddComponent(wall3,
                            Renderable{.model = getBoxModel(100, 50, 2), .program = terrainProgram, .texUnit = 1});
  // ceiling
  auto ceiling = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ceiling, Transform{.translation = T(-50, 50, -50), .rotation = Ry(0)});
  gCoordinator.AddComponent(ceiling,
                            Renderable{.model = getBoxModel(100, 2, 100), .program = terrainProgram, .texUnit = 1});
  // end load models

  printError("init");
}

void RenderSystem::Update() {
  drawSkybox();
  for (auto &entity : mEntities) {
    // printf("Entity id: %d\n", entity);
    Model *model = gCoordinator.GetComponent<Renderable>(entity).model;
    mat4 trans = gCoordinator.GetComponent<Transform>(entity).translation;
    mat4 rot = gCoordinator.GetComponent<Transform>(entity).rotation;
    GLuint program = gCoordinator.GetComponent<Renderable>(entity).program;
    GLuint texUnit = gCoordinator.GetComponent<Renderable>(entity).texUnit;
    glUseProgram(program);
    mat4 m2w = trans * rot;
    mat4 total = cameraMatrix * m2w;
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, m2w.m);
    glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), texUnit);
    DrawModel(model, terrainProgram, "inPosition", "inNormal", "inTexCoord");
  }
  drawGroundSphere();
  drawGround();
  auto &cameraTransform = gCoordinator.GetComponent<Transform>(0);
  auto &camera = gCoordinator.GetComponent<Camera>(0);
  cameraMatrix = lookAtv(cameraTransform.position, camera.cameraLookAt, camera.cameraUp);
}