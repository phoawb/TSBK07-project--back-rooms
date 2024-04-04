#define MAIN

#include "GL_utilities.h"
#include "LightManager.hpp"
#include "MicroGlut.h"
#include "boxes.h"
#include "components/Camera.hpp"
#include "components/Renderable.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "core/Enums.hpp"
#include "ground.h"
#include "systems/CameraControlSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "vector"

vec2 GROUND_SIZE = vec2(50, 100);

GLfloat t;
std::__1::shared_ptr<RenderSystem> renderSystem;
std::__1::shared_ptr<CameraControlSystem> cameraControlSystem;

Coordinator gCoordinator;

struct WallProps {
  int numWalls;
  std::vector<mat4> translations;
  std::vector<mat4> rotations;
  std::vector<vec3> dimensions;
};

void display(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  cameraControlSystem->Update(0.0f);
  renderSystem->Update();

  printError("display");

  glutSwapBuffers();
}

void mouse(int x, int y) {
  // This function is included in case you want some hints about using passive
  // mouse movement. Uncomment to see mouse coordinates: printf("%d %d\n", x,
  // y);
}

void onTimer(int value) {
  // pass time
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 200;
  // check input
  // checkInput();

  glutPostRedisplay();
  glutTimerFunc(20, &onTimer, value);  // 50 FPS
}

void createWallEntities(WallProps wallProps) {
  for (int i = 0; i < wallProps.numWalls; i++) {
    auto entity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(entity,
                              Transform{.translation = wallProps.translations[i], .rotation = wallProps.rotations[i]});
    gCoordinator.AddComponent(entity,
                              Renderable{.model = getBoxModel(wallProps.dimensions[i].x, wallProps.dimensions[i].y,
                                                              wallProps.dimensions[i].z),
                                         .shader = TERRAIN,
                                         .texUnit = 1});
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(600, 600);
  glutCreateWindow("TSBK07 - Project");
  glutDisplayFunc(display);

  // Start ECS stuff
  gCoordinator.Init();

  gCoordinator.RegisterComponent<Renderable>();
  gCoordinator.RegisterComponent<Transform>();
  gCoordinator.RegisterComponent<Camera>();

  cameraControlSystem = gCoordinator.RegisterSystem<CameraControlSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Camera>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    gCoordinator.SetSystemSignature<CameraControlSystem>(signature);
  }

  cameraControlSystem->Init();

  renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
  {
    Signature signature;
    signature.set(gCoordinator.GetComponentType<Renderable>());
    signature.set(gCoordinator.GetComponentType<Transform>());
    gCoordinator.SetSystemSignature<RenderSystem>(signature);
  }

  renderSystem->Init();

  int numWalls = 4;
  std::vector<mat4> translations = {T(-50, 0.0, -50), T(-50, 0, 50), T(-50, 0, -50), T(-50, 50, -50)};
  std::vector<mat4> rotations = {Ry(0), Ry(M_PI / 2), Ry(M_PI / 2), Ry(0)};
  std::vector<vec3> dimensions = {vec3(100, 50, 2), vec3(100, 50, 2), vec3(100, 50, 2), vec3(100, 2, 100)};

  WallProps wallProps = {numWalls, translations, rotations, dimensions};

  createWallEntities(wallProps);

  auto ground = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ground, Transform{.translation = T(0, 0, 0), .rotation = Ry(0)});
  gCoordinator.AddComponent(ground,
                            Renderable{.model = getGroundModel(GROUND_SIZE.x), .shader = TERRAIN, .texUnit = 0});

  auto groundSphere = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(groundSphere, Transform{.translation = T(0, 0, 0), .rotation = Ry(0)});
  gCoordinator.AddComponent(
      groundSphere, Renderable{.model = LoadModelPlus("objects/groundsphere.obj"), .shader = TERRAIN, .texUnit = 1});

  glutTimerFunc(20, &onTimer, 0);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
