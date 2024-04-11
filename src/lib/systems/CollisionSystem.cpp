#include "CollisionSystem.hpp"

#include "components/AABB.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void CollisionSystem::Init() { printf("CollisionSystem Init\n"); };

bool CollisionSystem::collissionPointAABB(vec3 p1, vec3 min2, vec3 max2) {
  if (p1.x > min2.x && p1.x < max2.x && p1.y > min2.y && p1.y < max2.y && p1.z > min2.z && p1.z < max2.z) {
    return true;
  }
  return false;
}

bool CollisionSystem::collisionAABB(vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
  if (min1.x < max2.x && max1.x > min2.x && min1.y < max2.y && max1.y > min2.y && min1.z < max2.z && max1.z > min2.z) {
    return true;
  }
  return false;
}

void CollisionSystem::Update() {
  for (auto const& firstEntity : mEntities) {
    auto& firstRigidBody = gCoordinator.GetComponent<RigidBody>(firstEntity);
    if (firstRigidBody.isStatic) {
      continue;
    } else {
      auto& firstTransform = gCoordinator.GetComponent<Transform>(firstEntity);
      auto const& firstAabb = gCoordinator.GetComponent<AABB>(firstEntity);

      for (auto const& secondEntity : mEntities) {
        if (firstEntity == secondEntity) {
          continue;
        }
        auto& secondTransform = gCoordinator.GetComponent<Transform>(secondEntity);
        auto const& secondAabb = gCoordinator.GetComponent<AABB>(secondEntity);

        vec3 halfDims = firstAabb.dimensions / 2.0f;

        vec3 min1 = firstAabb.centered ? trans2pos(firstTransform.translation) - halfDims
                                       : trans2pos(firstTransform.translation);
        vec3 max1 = firstAabb.centered ? min1 + halfDims : min1 + firstAabb.dimensions;

        vec3 min2 = trans2pos(secondTransform.translation);
        vec3 max2 = min2 + secondAabb.dimensions;

        if (collisionAABB(min1, max1, min2, max2)) {
          firstTransform.translation.m[3] -= firstRigidBody.velocity.x;
          firstTransform.translation.m[7] -= firstRigidBody.velocity.y;
          firstTransform.translation.m[11] -= firstRigidBody.velocity.z;
          firstRigidBody.velocity = vec3(0.0f, 0.0f, 0.0f);
          firstRigidBody.acceleration = vec3(0.0f, 0.0f, 0.0f);
        }
      }
    }
  }
};
