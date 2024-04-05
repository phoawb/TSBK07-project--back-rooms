#include "CameraControlSystem.hpp"

#include "MicroGlut.h"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void CameraControlSystem::Update(int deltaMouseX, int deltaMouseY) {
  const float moveSpeed = 0.6f;
  const float rotationSpeed = 0.02f;
  const float mouseSensitivity = 0.01f;

  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& camera = gCoordinator.GetComponent<Camera>(entity);

    // Update camera position based on WASD keys
    vec3 forward = normalize(camera.lookAt - transform.position);  // Direction camera is facing
    forward.y = 0;                                                 // Remove y-component
    forward = normalize(forward);                                  // Normalize since we altered the length
    vec3 rightDir = normalize(cross(forward, camera.cameraUp));    // Right direction relative to camera's forward

    // Forward and backward
    if (glutKeyIsDown('w')) transform.position += forward * moveSpeed;
    if (glutKeyIsDown('s')) transform.position -= forward * moveSpeed;
    // Right and left (strafe)
    if (glutKeyIsDown('a')) transform.position -= rightDir * moveSpeed;
    if (glutKeyIsDown('d')) transform.position += rightDir * moveSpeed;

    // printf("Entity id: %d\n", entity);
    // printf("CCS::Camera position: %f %f %f\n", cameraPos.x, cameraPos.y, cameraPos.z);

    // Update camera orientation based on IJKL keys
    // Rotate left and right (around the up axis)
    if (glutKeyIsDown('j')) camera.theta -= rotationSpeed;
    if (glutKeyIsDown('l')) camera.theta += rotationSpeed;

    // Rotate up and down (around the right axis)
    if (glutKeyIsDown('i')) camera.phi += rotationSpeed;
    if (glutKeyIsDown('k')) camera.phi -= rotationSpeed;

    if (glutKeyIsDown(GLUT_KEY_UP)) {
      transform.position.y += moveSpeed;
    } else if (glutKeyIsDown(GLUT_KEY_DOWN)) {
      transform.position.y -= moveSpeed;
    }

    // mouse
    camera.theta += deltaMouseX * mouseSensitivity;
    camera.phi -= deltaMouseY * mouseSensitivity;

    // Clamp the rotation angle to be within reasonable values
    camera.phi = fmax(-M_PI_2 + 0.01, fmin(M_PI_2 - 0.01, camera.phi));
    // make sure theta stays within 0 to 2pi and is positive
    camera.theta = fmod(camera.theta, 2 * M_PI);  // fmax(0.f, fmod(theta, 2 * M_PI));

    // Ensure theta is within 0 to 2pi
    if (camera.theta < 0) {
      camera.theta += 2 * M_PI;
    } else if (camera.theta > 2 * M_PI) {
      camera.theta -= 2 * M_PI;
    }

    // Calculate new camera target based on orientation
    camera.lookAt.x = transform.position.x + cos(camera.phi) * cos(camera.theta);
    camera.lookAt.y = transform.position.y + sin(camera.phi);
    camera.lookAt.z = transform.position.z + cos(camera.phi) * sin(camera.theta);

    camera.matrix = lookAtv(transform.position, camera.lookAt, camera.cameraUp);
  }
}

void CameraControlSystem::InputListener(Event& event) {
  // mButtons = event.GetParam<std::bitset<8>>(Events::Window::Input::INPUT);
}
