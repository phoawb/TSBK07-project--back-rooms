#include "CollisionSystem.hpp"

#include "components/AABB.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void CollisionSystem::Init() { printf("CollisionSystem Init\n"); };

void CollisionSystem::Update() {
  int CAMERA_ENTITY_ID = 0;  // trust me lol
  auto& cameraTransform = gCoordinator.GetComponent<Transform>(CAMERA_ENTITY_ID);
  auto& cameraRigidBody = gCoordinator.GetComponent<RigidBody>(CAMERA_ENTITY_ID);
  vec3 cameraPos =
      vec3(cameraTransform.translation.m[3], cameraTransform.translation.m[7], cameraTransform.translation.m[11]);

  for (auto const& entity : mEntities) {
    auto& aabb = gCoordinator.GetComponent<AABB>(entity);
    if (cameraPos.x > aabb.minPoint.x && cameraPos.x < aabb.maxPoint.x && cameraPos.y > aabb.minPoint.y &&
        cameraPos.y < aabb.maxPoint.y && cameraPos.z > aabb.minPoint.z && cameraPos.z < aabb.maxPoint.z) {
      cameraTransform.translation.m[3] += -cameraRigidBody.velocity.x;
      cameraTransform.translation.m[7] += -cameraRigidBody.velocity.y;
      cameraTransform.translation.m[11] += -cameraRigidBody.velocity.z;
    }
  }
};
