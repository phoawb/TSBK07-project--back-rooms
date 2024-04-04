#include "LightingSystem.hpp"

#include "ShaderManager.hpp"
#include "components/Light.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;
extern ShaderManager shaderManager;

void LightingSystem::Init() {
  for (auto const& entity : mEntities) {
    printf("LightingSystem entity: %d\n", entity);
    auto& light = gCoordinator.GetComponent<Light>(entity);
    auto& transform = gCoordinator.GetComponent<Transform>(entity);

    auto shaderId = shaderManager.getShaderId(light.shader);
    printf("LIGHT: shaderId: %d\n", shaderId);

    glUseProgram(shaderId);
    int lightCount = 1;
    glUniform1i(glGetUniformLocation(shaderId, "lightCount"), lightCount);
    glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesColors"), lightCount, &light.color.x);
    glUniform3fv(glGetUniformLocation(shaderId, "lightSourcesDirPos"), lightCount, &transform.position.x);
    glUniform1iv(glGetUniformLocation(shaderId, "isDirectional"), lightCount, &light.isDirectional);
  }
};

void LightingSystem::Update() {
  /*
  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& light = gCoordinator.GetComponent<Light>(entity);
  }
  */
};
