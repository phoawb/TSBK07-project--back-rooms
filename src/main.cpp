#define MAIN

#include "AssetManager.hpp"
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "SimpleGUI.h"
#include "components/AABB.hpp"
#include "components/Camera.hpp"
#include "components/Gravity.hpp"
#include "components/Light.hpp"
#include "components/Renderable.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "core/Enums.hpp"
#include "genMap.h"
#include "mapCreator.h"
#include "systems/CameraControlSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/GuiSystem.hpp"
#include "systems/LightingSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "vector"

int WINDOW_SIZE = 600;

std::__1::shared_ptr<RenderSystem> renderSystem;
std::__1::shared_ptr<CameraControlSystem> cameraControlSystem;
std::__1::shared_ptr<LightingSystem> lightingSystem;
std::__1::shared_ptr<CollisionSystem> collisionSystem;
std::__1::shared_ptr<PhysicsSystem> physicsSystem;
std::__1::shared_ptr<GuiSystem> guiSystem;

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
  guiSystem->Update();
  lightingSystem->Update();

  printError("display");
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

void spawnBall(mat4 startTrans, vec3 velocity) {
  auto groundSphere = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(groundSphere, Transform{.translation = startTrans, .scale = S(1, 1, 1), .rotation = Ry(0)});
  gCoordinator.AddComponent(groundSphere,
                            RigidBody{.velocity = velocity, .acceleration = vec3(0.0f, 0.0f, 0.0f), .isStatic = false});
  auto groundSphereModel = assetManager.getModel(ModelType::SPHERE);
  gCoordinator.AddComponent(groundSphere, Renderable{.model = groundSphereModel, .shader = TERRAIN, .texture = GRASS});
  gCoordinator.AddComponent(groundSphere, AABB{.dimensions = vec3(2, 2, 2)});
  gCoordinator.AddComponent(groundSphere, Gravity{.acceleration = vec3(0.0f, -0.03f, 0.0f)});
}

void spawnCarton(mat4 startTrans, vec3 velocity) {
  auto carton = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(carton, Transform{.translation = startTrans, .scale = S(0.1, 0.1, 0.1), .rotation = Ry(0)});
  gCoordinator.AddComponent(carton,
                            RigidBody{.velocity = velocity, .acceleration = vec3(0.0f, 0.0f, 0.0f), .isStatic = false});
  auto cartonModel = assetManager.getModel(ModelType::CARTON);
  gCoordinator.AddComponent(carton, Renderable{.model = cartonModel, .shader = TERRAIN, .texture = CARTON_TEX});
  gCoordinator.AddComponent(carton, AABB{.dimensions = vec3(2, 2, 2)});
  gCoordinator.AddComponent(carton, Gravity{.acceleration = vec3(0.0f, -0.03f, 0.0f)});
}

void spawnChair(mat4 startTrans, vec3 velocity) {
  auto chair = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(
      chair, Transform{.translation = startTrans, .scale = S(20.0f, 20.0f, 20.0f), .rotation = Rz(2 * M_2_PI)});
  gCoordinator.AddComponent(chair,
                            RigidBody{.velocity = velocity, .acceleration = vec3(0.0f, 0.0f, 0.0f), .isStatic = false});
  auto chairModel = assetManager.getModel(ModelType::CHAIR);
  gCoordinator.AddComponent(chair, Renderable{.model = chairModel, .shader = TERRAIN, .texture = TextureType::WHITE});
  gCoordinator.AddComponent(chair, AABB{.dimensions = vec3(1, 1, 1), .isCentered = true});
  gCoordinator.AddComponent(chair, Gravity{.acceleration = vec3(0.0f, -0.03f, 0.0f)});
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
  gCoordinator.RegisterComponent<Gravity>();

  guiSystem = gCoordinator.RegisterSystem<GuiSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<Camera>());
    gCoordinator.SetSystemSignature<GuiSystem>(signature);
  }
  guiSystem->Init();

  cameraControlSystem = gCoordinator.RegisterSystem<CameraControlSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Camera>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<RigidBody>());
    signature.set(gCoordinator.GetComponentType<Gravity>());
    gCoordinator.SetSystemSignature<CameraControlSystem>(signature);
  }

  physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Transform>());
    signature.set(gCoordinator.GetComponentType<RigidBody>());
    signature.set(gCoordinator.GetComponentType<Gravity>());
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
  lightingSystem->Init();

  // genMap();
  MapCreator mapCreator(1000, 1000, 60, MIN_ROOM_SIZE, MIN_ROOM_SIZE);
  mapCreator.start();

  spawnBall(T(-95, 10, 50), vec3(0.f, 0.3f, 0.0f));
  spawnBall(T(-85, 10, 50), vec3(0.f, 0.5f, 0.0f));
  spawnChair(T(-75, 10, 50), vec3(0.f, 0.5f, 0.0f));
  spawnCarton(T(-75, 10, 40), vec3(0.f, 0.5f, 0.0f));

  glutRepeatingTimer(20);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
