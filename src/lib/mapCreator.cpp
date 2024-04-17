#include "mapCreator.h"

#include <memory>
#include <queue>
#include <vector>

#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "algorithm"
#include "boxes.h"
#include "components/AABB.hpp"
#include "components/Renderable.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "mapGeneration/BinarySpacePartitioner.h"
#include "mapGeneration/mapUtils.h"
#include "random"
#include "stdio.h"

extern Coordinator gCoordinator;

vec2 generateBottomLeftCornerBetween(vec2 boundaryLeftPoint, vec2 boundaryRightPoint, float pointModifier, int offset) {
  int minX = boundaryLeftPoint.x + offset;
  int maxX = boundaryRightPoint.x - offset;
  int minY = boundaryLeftPoint.y + offset;
  int maxY = boundaryRightPoint.y - offset;

  int xModifier = (maxX - minX) * pointModifier;
  int yModifier = (maxY - minY) * pointModifier;
  return vec2(randRange(minX, minX + xModifier), randRange(minY, minY + yModifier));
};

vec2 generateTopRightCornerBetween(vec2 boundaryLeftPoint, vec2 boundaryRightPoint, float pointModifier, int offset) {
  int minX = boundaryLeftPoint.x + offset;
  int maxX = boundaryRightPoint.x - offset;
  int minY = boundaryLeftPoint.y + offset;
  int maxY = boundaryRightPoint.y - offset;

  int xModifier = (maxX - minX) * pointModifier;
  int yModifier = (maxY - minY) * pointModifier;
  return vec2(randRange(minX + xModifier, maxX), randRange(minY + yModifier, maxY));
};

std::vector<NodePtr> findLeafNodes(const NodePtr& rootNode) {
  std::vector<NodePtr> leafNodes;
  if (!rootNode) return leafNodes;  // Return empty if the root is nullptr

  std::queue<NodePtr> queue;
  queue.push(rootNode);

  while (!queue.empty()) {
    auto currentNode = queue.front();
    queue.pop();

    // If a node has no children, it's a leaf node
    if (currentNode->children.empty()) {
      leafNodes.push_back(currentNode);
    } else {
      // Otherwise, enqueue its children for further exploration
      for (auto& child : currentNode->children) {
        queue.push(child);
      }
    }
  }

  return leafNodes;
}

std::vector<NodePtr> RoomGenerator::generateRooms(std::vector<NodePtr> roomSpaces, float pointModifierBottom,
                                                  float pointModifierTop, int offset) {
  std::vector<NodePtr> rooms;
  for (auto roomSpace : roomSpaces) {
    vec2 newBottomLeftCorner = generateBottomLeftCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner,
                                                               pointModifierBottom, offset);
    vec2 newTopRightCorner =
        generateTopRightCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner, pointModifierTop, offset);
    roomSpace->bottomLeftCorner = newBottomLeftCorner;
    roomSpace->topRightCorner = newTopRightCorner;
    roomSpace->bottomRightCorner = vec2(newTopRightCorner.x, newBottomLeftCorner.y);
    roomSpace->topLeftCorner = vec2(newBottomLeftCorner.x, newTopRightCorner.y);
    rooms.push_back(roomSpace);
  }
  return roomSpaces;
};

MapGenerator::MapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

std::vector<NodePtr> MapGenerator::calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
  BinarySpacePartitioner bsp(mapWidth, mapHeight);
  allNodesCollection = bsp.prepareNodesCollection(maxIterations, minRoomWidth, minRoomHeight);
  std::vector<NodePtr> roomSpaces = findLeafNodes(bsp.rootNode);
  RoomGenerator roomGenerator = RoomGenerator();
  std::vector<NodePtr> rooms = roomGenerator.generateRooms(roomSpaces);

  return rooms;
};

void MapCreator::start() { createMap(); }

void MapCreator::createMap() {
  MapGenerator mapGenerator(mapWidth, mapHeight);
  std::vector<NodePtr> map = mapGenerator.calculateMap(maxIterations, minRoomWidth, minRoomHeight);
  RoomCreator roomCreator = RoomCreator();
  for (NodePtr biome : map) {
    // createFloorModel(biome->bottomLeftCorner, biome->getWidth(), biome->getHeight());
    roomCreator.createRandPillarRoom(biome);
  }

  // create floor
  float floorAndCeilingThickness = 4;
  Model* floorModel = getBoxModel(mapWidth, floorAndCeilingThickness, mapHeight, 1);
  auto floor = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(floor, Transform{.translation = T({0.f, -floorAndCeilingThickness, 0.f})});
  gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = OFFICE_FLOOR});
  gCoordinator.AddComponent(floor, AABB{.dimensions = vec3(mapWidth, floorAndCeilingThickness, mapHeight)});
  gCoordinator.AddComponent(floor, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});

  // create ceiling
  Model* ceilingModel = getBoxModel(mapWidth, floorAndCeilingThickness, mapHeight, 20);
  auto ceiling = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ceiling, Transform{.translation = T({0.f, MAP_TALLNESS, 0.f})});
  gCoordinator.AddComponent(ceiling, Renderable{.model = ceilingModel, .shader = TERRAIN, .texture = OFFICE_CEILING});
  gCoordinator.AddComponent(ceiling, AABB{.dimensions = vec3(mapWidth, floorAndCeilingThickness, mapHeight)});
  gCoordinator.AddComponent(ceiling, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
}

void MapCreator::createFloorModel(vec2 origin, int width, int height, TextureType textureType) {
  Model* floorModel = getBoxModel(width, 4, height, 1);

  auto floor = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(floor, Transform{.translation = T(origin.y, 0, origin.x),
                                             Ry(0)});  // we define the floor's origin at the bottom left corner
  gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = textureType});
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
        Model* pillarModel = getBoxModel(pillarSize, roomTallness, pillarSize, 1);
        auto pillar = gCoordinator.CreateEntity();
        gCoordinator.AddComponent(pillar, Transform{.translation = T(world_x, 0, world_z)});
        gCoordinator.AddComponent(pillar, Renderable{.model = pillarModel, .shader = TERRAIN, .texture = OFFICE_WALL});
        gCoordinator.AddComponent(pillar, AABB{.dimensions = vec3(pillarSize, roomTallness, pillarSize)});
        gCoordinator.AddComponent(pillar, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
        // moldings for the pillars
      }
    }
  }
};
