#include "GuiSystem.hpp"

#include "AssetManager.hpp"
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "SimpleGUI.h"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "core/Enums.hpp"

extern Coordinator gCoordinator;

void GuiSystem::Init() {
  sgSetPosition(30, 30);
  sgSetTextColor(0, 0, 0);
  sgSetBackgroundColor(0.5, 0.5, 0.5, 0.5);
  sgSetFrameColor(0, 0, 0);
  sgCreateDisplayFloat(-1, -1, "Camera theta: ", &theta);
  sgCreateDisplayFloat(-1, -1, "Camera phi: ", &phi);
  sgCreateDisplayFloat(-1, -1, "Camera x: ", &cameraX);
  sgCreateDisplayFloat(-1, -1, "Camera y: ", &cameraY);
  sgCreateDisplayFloat(-1, -1, "Camera z: ", &cameraZ);
  printError("init GuiSystem");
}

void GuiSystem::Update() {
  for (auto &entity : mEntities) {
    auto &camera = gCoordinator.GetComponent<Camera>(entity);
    auto &transform = gCoordinator.GetComponent<Transform>(entity);
    theta = camera.theta;
    phi = camera.phi;
    cameraX = transform.translation.m[3];
    cameraY = transform.translation.m[7];
    cameraZ = transform.translation.m[11];
  }
  sgDraw();
}