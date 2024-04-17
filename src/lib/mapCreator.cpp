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
#include "random"
#include "stdio.h"

extern Coordinator gCoordinator;

// vec2 is mostly used for texture cordinates, so I havn't bothered defining any operations for it
/* typedef struct vec2 {
  int x, y;
  vec2() {}
  vec2(int x2, int y2) : x(x2), y(y2) {}
  bool operator==(const vec2& other) const { return x == other.x && y == other.y; }
} vec2, *vec2_ptr; */

int randRange(int min, int max) {
  if (min > max) std::swap(min, max);
  if (min == max) return min;
  static std::random_device rd;                         // Obtain a random number from hardware
  static std::mt19937 gen(rd());                        // Seed the generator
  std::uniform_int_distribution<> distr(min, max - 1);  // Define the range

  return distr(gen);
}

Node::Node(const vec2& bottomLeft, const vec2& topRight, int index) : treeLayerIndex(index) {
  bottomLeftCorner = bottomLeft;
  topRightCorner = topRight;
  bottomRightCorner = vec2(topRight.x, bottomLeft.y);
  topLeftCorner = vec2(bottomLeft.x, topRight.y);
};

void Node::addChild(std::shared_ptr<Node> child) {
  children.push_back(child);
  child->parent = shared_from_this();
}

int Node::getWidth() { return topRightCorner.x - bottomLeftCorner.x; }
int Node::getHeight() { return topRightCorner.y - bottomLeftCorner.y; }

Line::Line(Orientation orientation, vec2 coordinates) : orientation(orientation), coordinates(coordinates){};

BinarySpacePartitioner::BinarySpacePartitioner(int mapWidth, int mapHeight) {
  rootNode = std::make_shared<Node>(vec2(0, 0), vec2(mapWidth, mapHeight), 0);
};

vec2 BinarySpacePartitioner::getCoordinatesForOrientation(Orientation orientation, vec2 bottomLeftCorner,
                                                          vec2 topRightCorner, int minRoomWidth, int minRoomHeight) {
  if (orientation == HORIZONTAL) {
    return vec2(0, randRange((bottomLeftCorner.y + minRoomHeight), (topRightCorner.y - minRoomHeight)));
  }
  return vec2(randRange((bottomLeftCorner.x + minRoomWidth), (topRightCorner.x - minRoomWidth)), 0);
}

Line BinarySpacePartitioner::GetLineDividingSpace(vec2 bottomLeftCorner, vec2 topRightCorner, int minRoomWidth,
                                                  int minRoomHeight) {
  Orientation orientation;
  bool heightStatus = (topRightCorner.y - bottomLeftCorner.y) >= 2 * minRoomHeight;
  bool widthStatus = (topRightCorner.x - bottomLeftCorner.x) >= 2 * minRoomWidth;
  if (heightStatus && widthStatus) {
    orientation = rand() % 2 == 0 ? HORIZONTAL : VERTICAL;
  } else if (widthStatus) {
    orientation = VERTICAL;
  } else if (heightStatus) {
    orientation = HORIZONTAL;
  } else {
    return Line(NONE, vec2(0, 0));
  }

  Line returnLine = Line(orientation, getCoordinatesForOrientation(orientation, bottomLeftCorner, topRightCorner,
                                                                   minRoomWidth, minRoomHeight));
  return returnLine;
}

void BinarySpacePartitioner::splitTheSpace(NodePtr currentNode, int minRoomWidth, int minRoomHeight) {
  Line line =
      GetLineDividingSpace(currentNode->bottomLeftCorner, currentNode->topRightCorner, minRoomWidth, minRoomHeight);
  NodePtr node1, node2;
  if (line.orientation == NONE) return;  // Early return if the line is not valid
  if (line.orientation == HORIZONTAL) {
    node1 =
        std::make_shared<Node>(currentNode->bottomLeftCorner, vec2(currentNode->topRightCorner.x, line.coordinates.y),
                               currentNode->treeLayerIndex + 1);
    node2 = std::make_shared<Node>(vec2(currentNode->bottomLeftCorner.x, line.coordinates.y),
                                   currentNode->topRightCorner, currentNode->treeLayerIndex + 1);
  } else {  // VERTICAL
    node1 =
        std::make_shared<Node>(currentNode->bottomLeftCorner, vec2(line.coordinates.x, currentNode->topRightCorner.y),
                               currentNode->treeLayerIndex + 1);
    node2 = std::make_shared<Node>(vec2(line.coordinates.x, currentNode->bottomLeftCorner.y),
                                   currentNode->topRightCorner, currentNode->treeLayerIndex + 1);
  }

  currentNode->addChild(node1);
  currentNode->addChild(node2);
}

std::vector<NodePtr> BinarySpacePartitioner::collectAllNodesIteratively(NodePtr inputRootNode) {
  std::vector<NodePtr> allNodes;
  if (!inputRootNode) return allNodes;  // Early return if the root is null
  std::queue<NodePtr> toProcess;
  toProcess.push(inputRootNode);

  while (!toProcess.empty()) {
    auto currentNode = toProcess.front();
    toProcess.pop();

    allNodes.push_back(currentNode);  // Process the current node

    // Enqueue all children of the current node for processing
    for (auto& child : currentNode->children) {
      toProcess.push(child);
    }
  }
  return allNodes;
}

std::vector<NodePtr> BinarySpacePartitioner::prepareNodesCollection(int maxIterations, int minRoomWidth,
                                                                    int minRoomHeight) {
  std::queue<NodePtr> graph;
  graph.push(rootNode);
  int iterations = 0;
  while (iterations < maxIterations && !graph.empty()) {
    auto currentNode = graph.front();
    graph.pop();
    splitTheSpace(currentNode, minRoomWidth, minRoomHeight);

    // Enqueue children for further processing
    for (auto& child : currentNode->children) {
      graph.push(child);
    }
    iterations++;
  }

  // Collect all nodes after splitting
  std::vector<NodePtr> allNodes = collectAllNodesIteratively(rootNode);
  return allNodes;
};

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
        printf("Creating pillar at (%f, %f)\n", world_x, world_z);
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
