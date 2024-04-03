#include "renderSystem.hpp"

#include "GL_utilities.h"

extern Coordinator gCoordinator;

void RenderSystem::Init() {
  /*
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  printError("GL inits");

  */
  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);

  mCamera = gCoordinator.CreateEntity();
  printf("The entity id is %d\n", mCamera);

  gCoordinator.AddComponent(mCamera, Transform{.position = vec3(10.0f, 0.0f, 100.0f)});
  gCoordinator.AddComponent(mCamera, Camera{.projectionTransform = projectionMatrix,
                                            .cameraLookAt = vec3(0.0f, 10.0f, 0.0f),
                                            .cameraUp = vec3(0.0f, 1.0f, 0.0f)});

  /*
  vec3 angles = cameraDirection(theta, phi);
  cameraLookAt = cameraPos + angles;
  cameraMatrix = lookAtv(cameraPos, cameraLookAt, cameraUp);

  initShaders();
  uploadProjectionMatrix();

  placeLight(vec3(0, 40, 25), vec3(1, 1, 1));
  uploadLights();
  printError("init light");

  loadTextures();
  loadModels();
  printError("init");
  */
}

void RenderSystem::Update() {
  for (auto& entity : mEntities) {
    // printf("RenderSystem::Update\n");
  }
}