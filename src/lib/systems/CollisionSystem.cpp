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

        vec3 min1;
        if (firstAabb.isCamera) {
          min1.x = trans2pos(firstTransform.translation).x - firstAabb.dimensions.x / 2.0f;
          min1.y = trans2pos(firstTransform.translation).y - firstAabb.dimensions.y;
          min1.z = trans2pos(firstTransform.translation).z - firstAabb.dimensions.z / 2.0f;
        } else if (firstAabb.isCentered) {
          min1 = trans2pos(firstTransform.translation) - firstAabb.dimensions / 2.0f;
        } else {
          min1 = trans2pos(firstTransform.translation);
        }

        vec3 max1;
        if (firstAabb.isCamera) {
          max1.x = trans2pos(firstTransform.translation).x + firstAabb.dimensions.x / 2.0f;
          max1.y = trans2pos(firstTransform.translation).y;
          max1.z = trans2pos(firstTransform.translation).z + firstAabb.dimensions.z / 2.0f;
        } else if (firstAabb.isCentered) {
          max1 = trans2pos(firstTransform.translation) + firstAabb.dimensions / 2.0f;
        } else {
          max1 = min1 + firstAabb.dimensions;
        }

        vec3 min2 = trans2pos(secondTransform.translation);
        vec3 max2 = min2 + secondAabb.dimensions;

        if (collisionAABB(min1, max1, min2, max2)) {
          float penetrationDepthXright = max1.x - min2.x;
          float penetrationDepthXleft = max2.x - min1.x;
          float minPenetrationDepthX = std::min(penetrationDepthXright, penetrationDepthXleft);

          float penetrationDepthYtop = max1.y - min2.y;
          float penetrationDepthYbottom = max2.y - min1.y;
          float minPenetrationDepthY = std::min(penetrationDepthYtop, penetrationDepthYbottom);

          float penetrationDepthZback = max1.z - min2.z;
          float penetrationDepthZfront = max2.z - min1.z;
          float minPenetrationDepthZ = std::min(penetrationDepthZback, penetrationDepthZfront);
          // Determine the collision direction based on the smallest penetration depth
          if (minPenetrationDepthX < minPenetrationDepthY && minPenetrationDepthX < minPenetrationDepthZ) {
            // Horizontal collision on X-axis
            if (penetrationDepthXright < penetrationDepthXleft) {
              firstTransform.translation.m[3] -= minPenetrationDepthX;
            } else {
              firstTransform.translation.m[3] += minPenetrationDepthX;
            }
            firstRigidBody.velocity.x = 0.0f;
          } else if (minPenetrationDepthY < minPenetrationDepthZ) {
            // Vertical collision on Y-axis
            firstRigidBody.velocity.y = 0.0f;
            if (penetrationDepthYtop < penetrationDepthYbottom) {
              firstTransform.translation.m[7] -= minPenetrationDepthY;
            } else {
              firstTransform.translation.m[7] += minPenetrationDepthY;
            }
          } else {
            // Collision on Z-axis
            firstRigidBody.velocity.z = 0.0f;
            if (penetrationDepthZback < penetrationDepthZfront) {
              firstTransform.translation.m[11] -= minPenetrationDepthZ;
            } else {
              firstTransform.translation.m[11] += minPenetrationDepthZ;
            }
          }
          // After resolving the collision, reset the velocity and acceleration if needed
          firstRigidBody.acceleration = vec3(0.0f, 0.0f, 0.0f);
        }
      }
    }
  }
};
