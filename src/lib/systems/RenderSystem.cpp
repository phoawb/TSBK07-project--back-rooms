#include "RenderSystem.hpp"

#include "AssetManager.hpp"
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "boxes.h"
#include "components/AABB.hpp"
#include "components/Camera.hpp"
#include "components/Gravity.hpp"
#include "components/Renderable.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "core/Enums.hpp"

extern Coordinator gCoordinator;
extern AssetManager assetManager;

void RenderSystem::drawSkybox() {
  auto skyboxModel = assetManager.getModel(ModelType::SKYBOX);
  auto shaderId = assetManager.getShaderId(ShaderType::NO_SHADE);
  auto texUnit = assetManager.getTexId(SKYBOX_SKY);

  glUseProgram(shaderId);

  glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

  // disable depth test and culling
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // camera matrix without translation
  mat4 skyBoxTransform = cameraMatrix;

  // remove translation
  skyBoxTransform.m[3] = 0;
  skyBoxTransform.m[7] = 0;
  skyBoxTransform.m[11] = 0;

  glUniformMatrix4fv(glGetUniformLocation(shaderId, "cameraMatrix"), 1, GL_TRUE, skyBoxTransform.m);

  mat4 trans = T(0.0f, -0.3f, 0.0f);
  mat4 rot = Ry(0);
  mat4 total = rot * trans;
  mat4 scale = S(20.0f, 20.0f, 20.0f);
  total = total * scale;

  glUniformMatrix4fv(glGetUniformLocation(shaderId, "mdlMatrix"), 1, GL_TRUE, total.m);
  glUniform1i(glGetUniformLocation(shaderId, "texUnit"), texUnit);
  DrawModel(skyboxModel, shaderId, "inPosition", NULL, "inTexCoord");

  // enable depth test and culling
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

bool RenderSystem::isOutsideRenderDistance(mat4 trans) {
  vec3 position = trans2pos(trans);
  vec3 cameraPos = trans2pos(gCoordinator.GetComponent<Transform>(mCamera).translation);
  float distance2FromCamera = (cameraPos.x - position.x) * (cameraPos.x - position.x) +
                              (cameraPos.y - position.y) * (cameraPos.y - position.y) +
                              (cameraPos.z - position.z) * (cameraPos.z - position.z);
  return (renderDistance * renderDistance < distance2FromCamera);
}

void RenderSystem::Init() {
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  printError("GL inits");

  renderDistance = 700.0f;
  float far = 5000.0f;
  float near = 0.2f;
  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, near, far);

  // Add camera and transform component to entity
  mCamera = gCoordinator.CreateEntity();

  printf("Camera entity: %d\n", mCamera);

  // Camera Attributes
  float cameraSide = 3;
  float cameraHeight = 9;
  mat4 cameraStartTranslation = T(50.0f, 10.0f, 50.0f);
  vec3 cameraDimensions = vec3(cameraSide, cameraHeight, cameraSide);

  // Add component to entity
  gCoordinator.AddComponent(mCamera, Transform{.translation = cameraStartTranslation});
  gCoordinator.AddComponent(mCamera, Camera{.projectionTransform = projectionMatrix,
                                            .theta = 0,
                                            .phi = 0,
                                            .lookAt = vec3(0.0f, 50.0f, 0.0f),
                                            .cameraUp = vec3(0.0f, 1.0f, 0.0f),
                                            .dimensions = cameraDimensions});
  gCoordinator.AddComponent(mCamera, AABB{.dimensions = cameraDimensions, .isCamera = true});
  gCoordinator.AddComponent(
      mCamera,
      RigidBody{.velocity = vec3(0.0f, 0.0f, 0.0f), .acceleration = vec3(0.0f, 0.0f, 0.0f), .isStatic = false});
  gCoordinator.AddComponent(mCamera, Gravity{.acceleration = vec3(0.0f, -0.03f, 0.0f)});

  printError("init RenderSystem");
}

void RenderSystem::runfilter(GLuint shader, FBOstruct *in1, FBOstruct *in2, FBOstruct *out) {
  glUseProgram(shader);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  // bind to 0 and 1 because those are coded for in the useFBO function
  glUniform1i(glGetUniformLocation(shader, "texUnit"), 0);
  glUniform1i(glGetUniformLocation(shader, "texUnit2"), 1);
  useFBO(out, in1, in2);
  auto squareModel = assetManager.getModel(ModelType::SQUARE);
  DrawModel(squareModel, shader, "in_Position", NULL, "in_TexCoord");
  glFlush();
}

void RenderSystem::Update() {
  auto fbo1 = assetManager.getFBO1();
  auto fbo2 = assetManager.getFBO2();
  auto fbo3 = assetManager.getFBO3();
  auto fbo4 = assetManager.getFBO4();

  // render to fbo1
  useFBO(fbo1, 0L, 0L);

  // Clear framebuffer and depth buffer
  glClearColor(0.1, 0.1, 0.3, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  drawSkybox();
  // Draw entities
  for (auto &entity : mEntities) {
    Model *model = gCoordinator.GetComponent<Renderable>(entity).model;
    mat4 trans = gCoordinator.GetComponent<Transform>(entity).translation;
    if (isOutsideRenderDistance(trans)) {
      continue;
    }
    mat4 rot = gCoordinator.GetComponent<Transform>(entity).rotation;
    mat4 scale = gCoordinator.GetComponent<Transform>(entity).scale;
    ShaderType shader = gCoordinator.GetComponent<Renderable>(entity).shader;
    TextureType texture = gCoordinator.GetComponent<Renderable>(entity).texture;
    mat4 m2w = trans * rot * scale;
    mat4 total = cameraMatrix * m2w;
    auto shaderId = assetManager.getShaderId(shader);
    int texUnit = assetManager.getTexId(texture);
    glUseProgram(shaderId);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "model2World"), 1, GL_TRUE, m2w.m);
    glUniform1i(glGetUniformLocation(shaderId, "texUnit"), texUnit);
    DrawModel(model, shaderId, "inPosition", "inNormal", "inTexCoord");
  }
  // Activate threshold shader
  auto thres = assetManager.getShaderId(THRES);
  runfilter(thres, fbo1, 0L, fbo2);

  // Activate lowpass shader
  auto lowpassX = assetManager.getShaderId(LOWPASS_X);
  auto lowpassY = assetManager.getShaderId(LOWPASS_Y);
  unsigned int amount = 30;
  for (unsigned int i = 0; i < amount; i++) {
    runfilter(lowpassY, fbo2, 0L, fbo3);
    runfilter(lowpassX, fbo3, 0L, fbo2);
  }

  // activate the add-shader
  auto add = assetManager.getShaderId(ADD);
  runfilter(add, fbo1, fbo2, fbo4);

  // Render final scene
  auto plaintextureshader = assetManager.getShaderId(PLAIN);
  glUseProgram(plaintextureshader);
  useFBO(0L, fbo4, fbo2);
  glClearColor(0.0, 0.0, 0.0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto squareModel = assetManager.getModel(ModelType::SQUARE);
  DrawModel(squareModel, plaintextureshader, "in_Position", NULL, "in_TexCoord");

  auto &cameraTransform = gCoordinator.GetComponent<Transform>(mCamera);
  auto &camera = gCoordinator.GetComponent<Camera>(mCamera);
  // upload camera position for phong reasons
  vec3 cameraPos = trans2pos(cameraTransform.translation);
  uploadUniformVec3ToShader(assetManager.getShaderId(ShaderType::TERRAIN), "cameraPos", cameraPos);
  cameraMatrix = camera.matrix;
}