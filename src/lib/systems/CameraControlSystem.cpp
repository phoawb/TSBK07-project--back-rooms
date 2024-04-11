#include "CameraControlSystem.hpp"

#include "MicroGlut.h"
#include "components/Camera.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "components/Gravity.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

bool mouseToggle = false;
// shift for run
bool prevShiftPressed = false;
bool moveSpeedToggle = false;
// space for jump
bool velocityAdded = false;
// g for gravity toggle
bool prevGravityPressed = false;
bool gravityOn = true;

void CameraControlSystem::Update(int deltaMouseX, int deltaMouseY) {
  float moveSpeed_xz;
  const float moveSpeed_y = 0.1f;
  const float rotationSpeed = 0.05f;
  const float mouseSensitivity = 0.003f;

  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& camera = gCoordinator.GetComponent<Camera>(entity);
    auto& rigidBody = gCoordinator.GetComponent<RigidBody>(entity);
    auto& gravity = gCoordinator.GetComponent<Gravity>(entity);

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
    if (camera.theta < 0)
      camera.theta += 2 * M_PI;
    else if (camera.theta > 2 * M_PI)
      camera.theta -= 2 * M_PI;

    // Update camera position based on WASD keys
    vec3 forward = normalize(camera.lookAt - cameraPosition);    // Direction camera is facing
    forward.y = 0;                                               // Remove y-component
    forward = normalize(forward);                                // Normalize since we altered the length
    vec3 rightDir = normalize(cross(forward, camera.cameraUp));  // Right direction relative to camera's forward

    rigidBody.velocity.x = 0;
    rigidBody.velocity.z = 0;

    // Gravity toggle
    bool currentGravityPressed = glutKeyIsDown('g');
    if (currentGravityPressed && !prevGravityPressed) {
      gravityOn = !gravityOn;
    }
    prevGravityPressed = currentGravityPressed; // Update the state for the next frame
    gravity.acceleration = gravityOn ? vec3(0.0f, -0.03f, 0.0f) : vec3(0.0f, 0.0f, 0.0f);

    // Shift for run
    bool currentShiftPressed = glutKeyIsDown(GLUT_KEY_SHIFT);
    if (currentShiftPressed && !prevShiftPressed) {
        moveSpeedToggle = !moveSpeedToggle;
    }
    moveSpeed_xz = moveSpeedToggle ? 1.0f : 0.7f;
    prevShiftPressed = currentShiftPressed; // Update the state for the next frame

    // Forward and backward
    if (glutKeyIsDown('w'))
      rigidBody.velocity += forward * moveSpeed_xz;
    else if (glutKeyIsDown('s'))
      rigidBody.velocity -= forward * moveSpeed_xz;

    // Left and right
    if (glutKeyIsDown('a')) rigidBody.velocity -= rightDir * moveSpeed_xz;
    if (glutKeyIsDown('d')) rigidBody.velocity += rightDir * moveSpeed_xz;

    if (glutKeyIsDown('m')) mouseToggle = !mouseToggle;

    // Update camera orientation based on IJKL keys
    // Rotate left and right (around the up axis)
    if (glutKeyIsDown('j')) camera.theta -= rotationSpeed;
    if (glutKeyIsDown('l')) camera.theta += rotationSpeed;

    // Rotate up and down (around the right axis)
    if (glutKeyIsDown('i')) camera.phi += rotationSpeed;
    if (glutKeyIsDown('k')) camera.phi -= rotationSpeed;

    if (glutKeyIsDown(GLUT_KEY_UP) || glutKeyIsDown('q')) rigidBody.velocity.y += moveSpeed_y;
    if (glutKeyIsDown(GLUT_KEY_DOWN) || glutKeyIsDown('e')) rigidBody.velocity.y -= moveSpeed_y;

    if (glutKeyIsDown(GLUT_KEY_SPACE)) {
      if (!velocityAdded) {
        rigidBody.velocity.y = 0.5f;
        velocityAdded = true;
      }
    } else {
      velocityAdded = false;
    }


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
