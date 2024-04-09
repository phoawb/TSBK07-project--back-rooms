#include "CollisionSystem.hpp"

#include "components/AABB.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void CollisionSystem::Init() { printf("CollisionSystem Init\n"); };

void CollisionSystem::Update() {
  int CAMERA_ENTITY_ID = 0;  // trust me lol
  vec3 cameraPos = gCoordinator.GetComponent<Transform>(CAMERA_ENTITY_ID).position;

  for (auto const& entity : mEntities) {
    auto& aabb = gCoordinator.GetComponent<AABB>(entity);
    if (cameraPos.x > aabb.minPoint.x && cameraPos.x < aabb.maxPoint.x && cameraPos.y > aabb.minPoint.y &&
        cameraPos.y < aabb.maxPoint.y && cameraPos.z > aabb.minPoint.z && cameraPos.z < aabb.maxPoint.z) {
      // Collision detected
    }
  }
};
