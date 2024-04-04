#include "LightingSystem.hpp"

#include "ShaderManager.hpp"
#include "components/Light.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;
extern ShaderManager shaderManager;

void LightingSystem::Init() {
  int lightCount = mEntities.size();
  vec3 lightSourcesColors[lightCount];
  vec3 lightSourcesDirPos[lightCount];
  int isDirectional[lightCount];
  int i = 0;
  for (auto const& entity : mEntities) {
    auto& light = gCoordinator.GetComponent<Light>(entity);
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    lightSourcesColors[i] = light.color;
    lightSourcesDirPos[i] = transform.position;
    isDirectional[i] = light.isDirectional;
    i++;
  }
  auto shaderId = shaderManager.getShaderId(TERRAIN);
  glUseProgram(shaderId);
  glUniform1i(glGetUniformLocation(shaderId, "lightCount"), lightCount);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesColors"), lightCount, &lightSourcesColors[0].x);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesDirPos"), lightCount, &lightSourcesDirPos[0].x);
  glUniform1iv(glGetUniformLocation(shaderId, "isDirectional"), lightCount, &isDirectional[0]);
};

void LightingSystem::Update() {
  // random number between 0 and 1
  float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  printf("random: %f\n", random);
  int lightCount = mEntities.size();
  vec3 lightSourcesColors[lightCount];
  vec3 lightSourcesDirPos[lightCount];
  int isDirectional[lightCount];
  int i = 0;
  for (auto const& entity : mEntities) {
    auto& light = gCoordinator.GetComponent<Light>(entity);
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    lightSourcesColors[i] = random < 0.99 ? light.color : vec3(0, 0, 0);
    lightSourcesDirPos[i] = transform.position;
    isDirectional[i] = light.isDirectional;
    i++;
  }
  auto shaderId = shaderManager.getShaderId(TERRAIN);
  glUseProgram(shaderId);
  glUniform1i(glGetUniformLocation(shaderId, "lightCount"), lightCount);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesColors"), lightCount, &lightSourcesColors[0].x);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesDirPos"), lightCount, &lightSourcesDirPos[0].x);
  glUniform1iv(glGetUniformLocation(shaderId, "isDirectional"), lightCount, &isDirectional[0]);
};
