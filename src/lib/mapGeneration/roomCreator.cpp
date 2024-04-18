#include "roomCreator.h"

#include "boxes.h"
#include "components/AABB.hpp"
#include "components/RigidBody.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

void createPillar(GLfloat world_x, GLfloat world_z, int pillarSize, int roomTallness) {
  Model* pillarModel = getBoxModel(pillarSize, roomTallness, pillarSize, 1);
  auto pillar = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(pillar, Transform{.translation = T(world_x, 0, world_z)});
  gCoordinator.AddComponent(pillar, Renderable{.model = pillarModel, .shader = TERRAIN, .texture = OFFICE_WALL});
  gCoordinator.AddComponent(pillar, AABB{.dimensions = vec3(pillarSize, roomTallness, pillarSize)});
  gCoordinator.AddComponent(pillar, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
  // moldings for the pillars (bot and top)
  for (float m = 0.f; m <= MAP_TALLNESS - MOLDING_HEIGHT; m += MAP_TALLNESS - MOLDING_HEIGHT) {
    Model* moldingModel =
        getBoxModel(pillarSize + MOLDING_DEPTH * 2, MOLDING_HEIGHT, pillarSize + MOLDING_DEPTH * 2, 1);
    auto molding = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(molding,
                              Transform{.translation = T(world_x - MOLDING_DEPTH, m, world_z - MOLDING_DEPTH)});
    gCoordinator.AddComponent(molding, Renderable{.model = moldingModel, .shader = TERRAIN, .texture = OFFWHITE});
    gCoordinator.AddComponent(molding, AABB{.dimensions = vec3(pillarSize + MOLDING_DEPTH * 2, MOLDING_HEIGHT,
                                                               pillarSize + MOLDING_DEPTH * 2)});
    gCoordinator.AddComponent(molding, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
  }
}

void createCeiling(int width, int height, float thickness) {
  Model* ceilingModel = getBoxModel(width, thickness, height, 20);
  auto ceiling = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ceiling, Transform{.translation = T({0.f, MAP_TALLNESS, 0.f})});
  gCoordinator.AddComponent(ceiling, Renderable{.model = ceilingModel, .shader = TERRAIN, .texture = OFFICE_CEILING});
  gCoordinator.AddComponent(ceiling, AABB{.dimensions = vec3(width, thickness, height)});
  gCoordinator.AddComponent(ceiling, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
}

void createFloor(int width, int height, float thickness) {
  Model* floorModel = getBoxModel(width, thickness, height, 1);
  auto floor = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(floor, Transform{.translation = T({0.f, -thickness, 0.f})});
  gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = OFFICE_FLOOR});
  gCoordinator.AddComponent(floor, AABB{.dimensions = vec3(width, thickness, height)});
  gCoordinator.AddComponent(floor, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
}

bool RoomCreator::isOccupied(std::vector<std::vector<TileType>>& grid, int x, int y) {
  if (grid[x][y] != TileType::EMPTY) return true;
  std::vector<vec2> directions = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                  {0, 2}, {0, -2}, {2, 0},  {-2, 0},  {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
                                  {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
  for (auto& direction : directions) {
    int newX = x + direction.x;
    int newY = y + direction.y;
    // check if out of bounds
    if (newX < 0 || newX >= (int)grid.size() || newY < 0 || newY >= (int)grid[0].size()) continue;
    // check if occupied
    if (grid[newX][newY] != TileType::EMPTY) return true;
  }
  return false;
}

void RoomCreator::createRandPillarRoom(NodePtr room, int pillarSize, int roomTallness) {
  int roomWidth = room->getWidth();
  int roomHeight = room->getHeight();
  int roomX = room->bottomLeftCorner.x;
  int roomY = room->bottomLeftCorner.y;
  int gridWidth = roomWidth / pillarSize;
  int gridHeight = roomHeight / pillarSize;
  std::vector<std::vector<TileType>> grid(gridWidth, std::vector<TileType>(gridHeight, EMPTY));

  // set seed
  srand(time(NULL));

  int maxPillars = gridWidth * gridHeight;
  int maxFailedTries = 1000;
  int failedTries = 0;
  int placedPillars = 0;
  while (placedPillars < maxPillars && failedTries < maxFailedTries) {
    int x = rand() % gridWidth;
    int y = rand() % gridHeight;
    if (isOccupied(grid, x, y)) {
      failedTries++;
      continue;
    }
    grid[x][y] = PILLAR;
    placedPillars++;
    failedTries = 0;
  }
  for (int y = 0; y < gridHeight; y++) {
    for (int x = 0; x < gridWidth; x++) {
      if (grid[x][y] == PILLAR) {
        GLfloat world_x = roomY + y * pillarSize;
        GLfloat world_z = roomX + x * pillarSize;
        createPillar(world_x, world_z, pillarSize, roomTallness);
      }
    }
  }
};