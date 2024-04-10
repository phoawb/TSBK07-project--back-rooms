#define MAIN

#include "AssetManager.hpp"
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "components/AABB.hpp"
#include "components/Camera.hpp"
#include "components/Light.hpp"
#include "components/Renderable.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "core/Enums.hpp"
#include "genMap.h"
#include "ground.h"
#include "systems/CameraControlSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/LightingSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "vector"

float GROUND_SIZE = 100;

GLfloat t;
std::__1::shared_ptr<RenderSystem> renderSystem;
std::__1::shared_ptr<CameraControlSystem> cameraControlSystem;
std::__1::shared_ptr<LightingSystem> lightingSystem;
std::__1::shared_ptr<CollisionSystem> collisionSystem;

Coordinator gCoordinator;
AssetManager assetManager;

int deltaMouseX = 0;
int deltaMouseY = 0;
int lastMouseX = 0;
int lastMouseY = 0;

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  cameraControlSystem->Update(deltaMouseX, deltaMouseY);
  renderSystem->Update();
  lightingSystem->Update();
  collisionSystem->Update();

  printError("display");
  glutSwapBuffers();
}

/// @brief Mouse callback function
/// @param x
/// @param y
void mouse(int x, int y) {
  deltaMouseX = x - lastMouseX;
  deltaMouseY = y - lastMouseY;
  lastMouseX = x;
  lastMouseY = y;
}

void createLightEntities() {
  int lightCount = 4;
  for (int i = 0; i < lightCount; i++) {
    int randomX = rand() % 100 - 50;
    int randomY = rand() % 100 - 50;
    /* int randomX = 0;
    int randomY = 0; */
    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 pos = vec3(randomX, 25, randomY);
    auto lightEntity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(lightEntity, Transform{.position = pos});
    gCoordinator.AddComponent(lightEntity, Light{.color = color, .shader = TERRAIN});
  }

  auto groundSphere2 = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(groundSphere2, Transform{.translation = T(-20, 0, 0), .rotation = Ry(0)});
  gCoordinator.AddComponent(
      groundSphere2,
      Renderable{.model = LoadModelPlus("objects/groundsphere.obj"), .shader = NO_SHADE, .texture = GRASS});
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(800, 800);
  glutCreateWindow("TSBK07 - Project");
  glutDisplayFunc(display);

  gCoordinator.Init();
  assetManager.Init();

  gCoordinator.RegisterComponent<Renderable>();
  gCoordinator.RegisterComponent<Transform>();
  gCoordinator.RegisterComponent<Camera>();
  gCoordinator.RegisterComponent<Light>();
  gCoordinator.RegisterComponent<AABB>();

  cameraControlSystem = gCoordinator.RegisterSystem<CameraControlSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Camera>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    gCoordinator.SetSystemSignature<CameraControlSystem>(signature);
  }

  renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Renderable>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    gCoordinator.SetSystemSignature<RenderSystem>(signature);
  }

  renderSystem->Init();

  lightingSystem = gCoordinator.RegisterSystem<LightingSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Light>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    gCoordinator.SetSystemSignature<LightingSystem>(signature);
  }
  createLightEntities();
  lightingSystem->Init();

  collisionSystem = gCoordinator.RegisterSystem<CollisionSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<AABB>());
    gCoordinator.SetSystemSignature<CollisionSystem>(signature);
  }
  collisionSystem->Init();

  genMap();

  auto ground = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ground, Transform{.translation = T(0, 0, 0), .rotation = Ry(0)});
  gCoordinator.AddComponent(
      ground, Renderable{.model = getGroundModel(GROUND_SIZE), .shader = TERRAIN, .texture = OFFICE_FLOOR});

  auto groundSphere = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(groundSphere, Transform{.translation = T(-10, 0, 0), .rotation = Ry(0)});
  auto groundSphereModel = assetManager.getModel(ModelType::SPHERE);
  gCoordinator.AddComponent(groundSphere, Renderable{.model = groundSphereModel, .shader = TERRAIN, .texture = GRASS});

  glutRepeatingTimer(20);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
