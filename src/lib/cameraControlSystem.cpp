#include "CameraControlSystem.hpp"

#include "MicroGlut.h"

extern Coordinator gCoordinator;

void CameraControlSystem::Init() {
  printf("CameraControlSystem::Init\n");
  // gCoordinator.AddEventListener(METHOD_LISTENER(Events::Window::INPUT, CameraControlSystem::InputListener));
}

void CameraControlSystem::Update(float dt) {
  for (auto& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& camera = gCoordinator.GetComponent<Camera>(entity);

    float cameraSpeed = 0.6f;     // Speed of camera movement
    float rotationSpeed = 0.02f;  // Speed of camera rotation

    // Update camera position based on WASD keys
    vec3 forward = normalize(camera.cameraLookAt - transform.position);  // Direction camera is facing
    forward.y = 0;                                                       // Remove y-component
    forward = normalize(forward);                                        // Normalize since we altered the length
    vec3 rightDir = normalize(cross(forward, camera.cameraUp));          // Right direction relative to camera's forward

    // Forward and backward
    if (glutKeyIsDown('w')) cameraPos += forward * cameraSpeed;
    if (glutKeyIsDown('s')) cameraPos -= forward * cameraSpeed;
    // Right and left (strafe)
    if (glutKeyIsDown('a')) cameraPos -= rightDir * cameraSpeed;
    if (glutKeyIsDown('d')) cameraPos += rightDir * cameraSpeed;

    transform.position = cameraPos;

    // printf("Entity id: %d\n", entity);
    // printf("CCS::Camera position: %f %f %f\n", cameraPos.x, cameraPos.y, cameraPos.z);

    // Update camera orientation based on IJKL keys
    // Rotate left and right (around the up axis)
    if (glutKeyIsDown('j')) theta -= rotationSpeed;
    if (glutKeyIsDown('l')) theta += rotationSpeed;

    // Rotate up and down (around the right axis)
    if (glutKeyIsDown('i')) phi += rotationSpeed;
    if (glutKeyIsDown('k')) phi -= rotationSpeed;

    if (glutKeyIsDown(GLUT_KEY_UP)) {
      cameraPos.y += cameraSpeed;
    } else if (glutKeyIsDown(GLUT_KEY_DOWN)) {
      cameraPos.y -= cameraSpeed;
    }

    // Clamp the rotation angle to be within reasonable values
    phi = fmax(-M_PI_2 + 0.01, fmin(M_PI_2 - 0.01, phi));
    // make sure theta stays within 0 to 2pi and is positive
    theta = fmod(theta, 2 * M_PI);  // fmax(0.f, fmod(theta, 2 * M_PI));

    // Calculate new camera target based on orientation
    camera.cameraLookAt.x = cameraPos.x + cos(phi) * cos(theta);
    camera.cameraLookAt.y = cameraPos.y + sin(phi);
    camera.cameraLookAt.z = cameraPos.z + cos(phi) * sin(theta);

    // Finally, update the camera matrix with the new position and target
    cameraMatrix = lookAtv(cameraPos, camera.cameraLookAt, camera.cameraUp);
  }
}

void CameraControlSystem::InputListener(Event& event) {
  // mButtons = event.GetParam<std::bitset<8>>(Events::Window::Input::INPUT);
}
