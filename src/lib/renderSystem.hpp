#pragma once

#include "ecs.hpp"
#include "memory"

class Event;

class RenderSystem : public System {
 public:
  void Init();

  void Update();

 private:
  // void WindowSizeListener(Event& event);

  // std::unique_ptr<Shader> shader;

  Entity mCamera;
  mat4 projectionMatrix;

  float theta = 0;
  float phi = 0;

  // GLuint mVao{};
  // GLuint mVboVertices{};
  // GLuint mVboNormals{};
};
