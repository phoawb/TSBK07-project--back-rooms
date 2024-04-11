#include "mapCreator.h"

#include <memory>
#include <queue>
#include <vector>

#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "algorithm"
#include "boxes.h"
#include "components/Renderable.hpp"
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

std::vector<NodePtr> RoomGenerator::generateRooms(std::vector<NodePtr> roomSpaces, float pointModifier) {
  std::vector<NodePtr> rooms;
  for (auto roomSpace : roomSpaces) {
    vec2 newBottomLeftCorner =
        generateBottomLeftCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner, pointModifier, 1);
    vec2 newTopRightCorner =
        generateTopRightCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner, 1 - pointModifier, 1);
    roomSpace->bottomLeftCorner = newBottomLeftCorner;
    roomSpace->topRightCorner = newTopRightCorner;
    roomSpace->bottomRightCorner = vec2(newTopRightCorner.x, newBottomLeftCorner.y);
    roomSpace->topLeftCorner = vec2(newBottomLeftCorner.x, newTopRightCorner.y);
    rooms.push_back(roomSpace);
  }
  return rooms;
};

MapGenerator::MapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

std::vector<NodePtr> MapGenerator::calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
  BinarySpacePartitioner bsp(mapWidth, mapHeight);
  allNodesCollection = bsp.prepareNodesCollection(maxIterations, minRoomWidth, minRoomHeight);
  std::vector<NodePtr> roomSpaces = findLeafNodes(bsp.rootNode);
  RoomGenerator roomGenerator = RoomGenerator();
  std::vector<NodePtr> rooms = roomGenerator.generateRooms(roomSpaces);

  return roomSpaces;
};

void MapCreator::start() { createMap(); }

void MapCreator::createMap() {
  MapGenerator mapGenerator(mapWidth, mapHeight);
  std::vector<NodePtr> map = mapGenerator.calculateMap(maxIterations, minRoomWidth, minRoomHeight);
  for (auto room : map) {
    createFloorModel(room->bottomLeftCorner, room->getWidth(), room->getHeight());
  }
}

void MapCreator::createFloorModel(vec2 origin, int width, int height, TextureType textureType) {
  Model* floorModel = getBoxModel(width, 4, height, 1);

  auto floor = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(floor, Transform{.translation = T(origin.y, 0, origin.x),
                                             Ry(0)});  // we define the floor's origin at the bottom left corner
  gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = textureType});
}