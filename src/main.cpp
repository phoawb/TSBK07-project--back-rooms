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
#include "mapCreator.h"
#include "systems/CameraControlSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/LightingSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "vector"

float GROUND_SIZE = 100;
int WINDOW_SIZE = 600;

GLfloat t;
std::__1::shared_ptr<RenderSystem> renderSystem;
std::__1::shared_ptr<CameraControlSystem> cameraControlSystem;
std::__1::shared_ptr<LightingSystem> lightingSystem;
std::__1::shared_ptr<CollisionSystem> collisionSystem;
std::__1::shared_ptr<PhysicsSystem> physicsSystem;

Coordinator gCoordinator;
AssetManager assetManager;

int deltaMouseX = 0;
int deltaMouseY = 0;
int lastMouseX = 0;
int lastMouseY = 0;

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  cameraControlSystem->Update(deltaMouseX, deltaMouseY);
  physicsSystem->Update();
  collisionSystem->Update();
  renderSystem->Update();

  lightingSystem->Update();

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
    vec3 color = vec3(1.0, 1.0, 1.0);
    mat4 trans = T(randomX, 25, randomY);
    auto lightEntity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(lightEntity, Transform{.translation = trans});
    gCoordinator.AddComponent(lightEntity, Light{.color = color, .shader = TERRAIN});
  }
}

void spawnBall(mat4 startTrans, vec3 velocity) {
  auto groundSphere = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(groundSphere, Transform{.translation = startTrans, .rotation = Ry(0)});
  gCoordinator.AddComponent(groundSphere,
                            RigidBody{.velocity = velocity, .acceleration = vec3(0.0f, 0.0f, 0.0f), .isStatic = false});
  auto groundSphereModel = assetManager.getModel(ModelType::SPHERE);
  gCoordinator.AddComponent(groundSphere, Renderable{.model = groundSphereModel, .shader = TERRAIN, .texture = GRASS});
  gCoordinator.AddComponent(groundSphere, AABB{.dimensions = vec3(2, 2, 2)});
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
  glutCreateWindow("TSBK07 - Project");
  glutDisplayFunc(display);

  gCoordinator.Init();
  assetManager.Init();

  gCoordinator.RegisterComponent<Renderable>();
  gCoordinator.RegisterComponent<Transform>();
  gCoordinator.RegisterComponent<Camera>();
  gCoordinator.RegisterComponent<Light>();
  gCoordinator.RegisterComponent<AABB>();
  gCoordinator.RegisterComponent<RigidBody>();

  cameraControlSystem = gCoordinator.RegisterSystem<CameraControlSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Camera>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<RigidBody>());
    gCoordinator.SetSystemSignature<CameraControlSystem>(signature);
  }

  physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<RigidBody>());
    gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
  }
  physicsSystem->Init();

  collisionSystem = gCoordinator.RegisterSystem<CollisionSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<RigidBody>());
    signature.set(gCoordinator.GetComponentType<AABB>());
    gCoordinator.SetSystemSignature<CollisionSystem>(signature);
  }
  collisionSystem->Init();

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

  // genMap();
  MapCreator mapCreator(1000, 1000, 50, 200, 200);
  mapCreator.start();

  // fly up
  spawnBall(T(-85, 5, 60), vec3(0.f, 0.2f, 0.0f));
  spawnBall(T(-75, 5, 60), vec3(0.f, 0.2f, 0.0f));

  // fly into each other
  spawnBall(T(-85, 5, 50), vec3(0.2f, 0.0f, 0.f));
  spawnBall(T(-75, 5, 50), vec3(-0.2f, 0.0f, 0.f));

  auto ground = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ground, Transform{.translation = T(0, 0, 0), .rotation = Ry(0)});
  gCoordinator.AddComponent(
      ground, Renderable{.model = getGroundModel(GROUND_SIZE), .shader = TERRAIN, .texture = OFFICE_FLOOR});

  glutRepeatingTimer(20);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
