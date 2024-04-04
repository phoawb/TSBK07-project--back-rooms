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
  /*
  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& light = gCoordinator.GetComponent<Light>(entity);
    auto shaderId = shaderManager.getShaderId(TERRAIN);
    glUseProgram(shaderId);
    int lightCount = 1;
    glUniform1i(glGetUniformLocation(shaderId, "lightCount"), lightCount);
    glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesColors"), lightCount, &light.color.x);
    glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesDirPos"), lightCount, &transform.position.x);
    glUniform1iv(glGetUniformLocation(shaderId, "isDirectional"), lightCount, &light.isDirectional);
  }
  */
};
