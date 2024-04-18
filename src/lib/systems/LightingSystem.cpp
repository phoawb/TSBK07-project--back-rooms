#include "LightingSystem.hpp"

#include "AssetManager.hpp"
#include "components/Light.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;
extern AssetManager assetManager;

void LightingSystem::Init() {};

void LightingSystem::Update() {
  // random number between 0 and 1
  // printf("random: %f\n", random);
  int lightCount = mEntities.size();
  vec3 lightSourcesColors[lightCount];
  vec3 lightSourcesDirPos[lightCount];
  int isDirectional[lightCount];
  int i = 0;
  for (auto const& entity : mEntities) {
    auto& light = gCoordinator.GetComponent<Light>(entity);
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    lightSourcesColors[i] = light.color;
    vec3 lightSourcePos = vec3(transform.translation.m[3], transform.translation.m[7], transform.translation.m[11]);
    lightSourcesDirPos[i] = lightSourcePos;
    isDirectional[i] = light.isDirectional;
    i++;
  }
  auto shaderId = assetManager.getShaderId(TERRAIN);
  glUseProgram(shaderId);
  glUniform1i(glGetUniformLocation(shaderId, "lightCount"), lightCount);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesColors"), lightCount, &lightSourcesColors[0].x);
  glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesDirPos"), lightCount, &lightSourcesDirPos[0].x);
  glUniform1iv(glGetUniformLocation(shaderId, "isDirectional"), lightCount, &isDirectional[0]);
};
