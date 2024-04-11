#include "CameraControlSystem.hpp"

#include "MicroGlut.h"
#include "components/Camera.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

bool mouseToggle = false;

void CameraControlSystem::Update(int deltaMouseX, int deltaMouseY) {
  const float moveSpeed = 1.0f;
  const float rotationSpeed = 0.05f;
  const float mouseSensitivity = 0.003f;

  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& camera = gCoordinator.GetComponent<Camera>(entity);
    auto& rigidBody = gCoordinator.GetComponent<RigidBody>(entity);

    vec3 cameraPosition = trans2pos(transform.translation);

    // Calculate camera target based on orientation
    camera.lookAt.x = cameraPosition.x + cos(camera.phi) * cos(camera.theta);
    camera.lookAt.y = cameraPosition.y + sin(camera.phi);
    camera.lookAt.z = cameraPosition.z + cos(camera.phi) * sin(camera.theta);

    // Update camera matrix
    camera.matrix = lookAtv(cameraPosition, camera.lookAt, camera.cameraUp);

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

    // Update camera position based on WASD keys
    vec3 forward = normalize(camera.lookAt - cameraPosition);    // Direction camera is facing
    forward.y = 0;                                               // Remove y-component
    forward = normalize(forward);                                // Normalize since we altered the length
    vec3 rightDir = normalize(cross(forward, camera.cameraUp));  // Right direction relative to camera's forward

    rigidBody.velocity = vec3(0, 0, 0);

    // Forward and backward
    if (glutKeyIsDown('w'))
      rigidBody.velocity += forward * moveSpeed;
    else if (glutKeyIsDown('s'))
      rigidBody.velocity -= forward * moveSpeed;

    // Left and right
    if (glutKeyIsDown('a')) rigidBody.velocity -= rightDir * moveSpeed;
    if (glutKeyIsDown('d')) rigidBody.velocity += rightDir * moveSpeed;

    if (glutKeyIsDown('m')) mouseToggle = !mouseToggle;

    // Update camera orientation based on IJKL keys
    // Rotate left and right (around the up axis)
    if (glutKeyIsDown('j')) camera.theta -= rotationSpeed;
    if (glutKeyIsDown('l')) camera.theta += rotationSpeed;

    // Rotate up and down (around the right axis)
    if (glutKeyIsDown('i')) camera.phi += rotationSpeed;
    if (glutKeyIsDown('k')) camera.phi -= rotationSpeed;

    if (glutKeyIsDown(GLUT_KEY_UP)) rigidBody.velocity.y += moveSpeed;
    if (glutKeyIsDown(GLUT_KEY_DOWN)) rigidBody.velocity.y -= moveSpeed;

    // mouse
    if (mouseToggle) {
      camera.theta = camera.theta + deltaMouseX * mouseSensitivity;  // this is yaw
      camera.phi -= deltaMouseY * mouseSensitivity;                  // this is pitch
    }
  }
}

void CameraControlSystem::InputListener(Event& event) {
  // mButtons = event.GetParam<std::bitset<8>>(Events::Window::Input::INPUT);
}
