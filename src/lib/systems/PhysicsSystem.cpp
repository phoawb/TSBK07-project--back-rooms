#include "PhysicsSystem.hpp"

#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void PhysicsSystem::Init() { printf("PhysicsSystem Init\n"); };
void PhysicsSystem::TranslationAdd(mat4& a, vec3 b) {
  a.m[3] += b.x;
  a.m[7] += b.y;
  a.m[11] += b.z;
};
void PhysicsSystem::Update() {
  for (auto const& entity : mEntities) {
    auto& transform = gCoordinator.GetComponent<Transform>(entity);
    auto& rigidBody = gCoordinator.GetComponent<RigidBody>(entity);
    TranslationAdd(transform.translation, rigidBody.velocity);
  }
};