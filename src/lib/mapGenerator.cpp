#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "algorithm"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"
#include "memory"
#include "queue"
#include "random"
#include "stdio.h"
#include "vector"

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
  static std::random_device rd;                     // Obtain a random number from hardware
  static std::mt19937 gen(rd());                    // Seed the generator
  std::uniform_int_distribution<> distr(min, max);  // Define the range

  return distr(gen);
}

class Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::weak_ptr<Node> WeakNodePtr;

class Node : public std::enable_shared_from_this<Node> {
 public:
  std::vector<NodePtr> children;
  bool visited;
  vec2 bottomLeftCorner, topLeftCorner, bottomRightCorner, topRightCorner;
  int treeLayerIndex;
  WeakNodePtr parent;  // Use weak_ptr to prevent circular references

  Node() : treeLayerIndex(0) {}

  Node(const vec2& bottomLeft, const vec2& topRight, int index) : treeLayerIndex(index) {
    bottomLeftCorner = bottomLeft;
    topRightCorner = topRight;
    bottomRightCorner = vec2(topRight.x, bottomLeft.y);
    topLeftCorner = vec2(bottomLeft.x, topRight.y);
  };

  void addChild(std::shared_ptr<Node> child) {
    children.push_back(child);
    child->parent = shared_from_this();
  }

  int getWidth() { return topRightCorner.x - bottomLeftCorner.x; }
  int getHeight() { return topRightCorner.y - bottomLeftCorner.y; }
};

enum Orientation {
  HORIZONTAL = 0,
  VERTICAL = 1,
};

class Line {
 public:
  Orientation orientation;
  vec2 coordinates;
  Line(Orientation orientation, vec2 coordinates) : orientation(orientation), coordinates(coordinates){};
};

class BinarySpacePartitioner {
 public:
  NodePtr rootNode;

  BinarySpacePartitioner(int mapWidth, int mapHeight) {
    rootNode = std::make_shared<Node>(vec2(0, 0), vec2(mapWidth, mapHeight), 0);
  };

  vec2 getCoordinatesForOrientation(Orientation orientation, vec2 bottomLeftCorner, vec2 topRightCorner,
                                    int minRoomWidth, int minRoomHeight) {
    if (orientation == HORIZONTAL) {
      return vec2(0, randRange((bottomLeftCorner.y + minRoomHeight), (topRightCorner.y - minRoomHeight)));
    }
    return vec2(randRange((bottomLeftCorner.x + minRoomWidth), (topRightCorner.x - minRoomWidth)), 0);
  }

  Line GetLineDividingSpace(vec2 bottomLeftCorner, vec2 topRightCorner, int minRoomWidth, int minRoomHeight) {
    Orientation orientation;
    bool heightStatus = (topRightCorner.y - bottomLeftCorner.y) >= 2 * minRoomHeight;
    bool widthStatus = (topRightCorner.x - bottomLeftCorner.x) >= 2 * minRoomWidth;
    if (heightStatus && widthStatus) {
      orientation = rand() % 2 == 0 ? HORIZONTAL : VERTICAL;
    } else if (widthStatus) {
      orientation = VERTICAL;
    } else {
      orientation = HORIZONTAL;
    }

    Line returnLine = Line(orientation, getCoordinatesForOrientation(orientation, bottomLeftCorner, topRightCorner,
                                                                     minRoomWidth, minRoomHeight));
    return returnLine;
  }

  void splitTheSpace(NodePtr currentNode, int minRoomWidth, int minRoomHeight) {
    Line line =
        GetLineDividingSpace(currentNode->bottomLeftCorner, currentNode->topRightCorner, minRoomWidth, minRoomHeight);
    NodePtr node1, node2;
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

  std::vector<NodePtr> collectAllNodesIteratively(NodePtr inputRootNode) {
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

  std::vector<NodePtr> prepareNodesCollection(int maxIterations, int minRoomWidth, int minRoomHeight) {
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
  }
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

class RoomGenerator {
 public:
  RoomGenerator(){};  // Default constructor

  std::vector<NodePtr> generateRooms(std::vector<NodePtr> roomSpaces, float pointModifier = 0.1) {
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
};

class MapGenerator {
 public:
  std::vector<NodePtr> allNodesCollection;
  int mapWidth, mapHeight;

  MapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

  std::vector<NodePtr> calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
    BinarySpacePartitioner bsp(mapWidth, mapHeight);
    allNodesCollection = bsp.prepareNodesCollection(maxIterations, minRoomWidth, minRoomHeight);
    std::vector<NodePtr> roomSpaces = findLeafNodes(bsp.rootNode);
    RoomGenerator roomGenerator = RoomGenerator();
    std::vector<NodePtr> rooms = roomGenerator.generateRooms(roomSpaces);
    return rooms;
  };
};

Model* getFloorModel(float groundSize) {
  vec3 vertices[] = {vec3(-groundSize, 0.0f, -groundSize), vec3(-groundSize, 0.0f, groundSize),
                     vec3(groundSize, -0.0f, -groundSize), vec3(groundSize, -0.0f, groundSize)};

  vec3 vertex_normals[] = {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                           vec3(0.0f, 1.0f, 0.0f)};

  vec2 tex_coords[] = {vec2(0.0f, 0.0f), vec2(0.0f, 20.0f), vec2(20.0f, 0.0f), vec2(20.0f, 20.0f)};
  GLuint indices[] = {0, 1, 2, 1, 3, 2};
  vec3 colors[] = {100.f, 0.f, 0.f};
  int numVert = 4, numInd = 6;
  Model* model = LoadDataToModel(vertices, vertex_normals, tex_coords, colors, indices, numVert, numInd);
  return model;
}

class MapCreator {
  int mapWidth, mapHeight;
  int maxIterations, minRoomWidth, minRoomHeight;
  // int corridorWidth;

  void start() { createMap(); }

  void createMap() {
    MapGenerator mapGenerator(mapWidth, mapHeight);
    std::vector<NodePtr> map = mapGenerator.calculateMap(maxIterations, minRoomWidth, minRoomHeight);
    for (auto room : map) {
    }
    // printf(map.size() > 0 ? "Map created successfully with %lu rooms\n" : "Map creation failed\n", map.size());
  }

  void createFloorModel(vec2 bottomLeftCorner, vec2 topRightCorner, TextureType textureType = OFFICE_FLOOR) {
    vec3 bottomLeftVertex = vec3(bottomLeftCorner.y, 0, bottomLeftCorner.x);
    vec3 bottomRightVertex = vec3(topRightCorner.y, 0, bottomLeftCorner.x);
    vec3 topLeftVertex = vec3(bottomLeftCorner.y, 0, topRightCorner.x);
    vec3 topRightVertex = vec3(topRightCorner.y, 0, topRightCorner.x);
    vec3 vertices[] = {bottomLeftVertex, bottomRightVertex, topLeftVertex, topRightVertex};
    vec3 vertex_normals[] = {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                             vec3(0.0f, 1.0f, 0.0f)};
    vec2 tex_coords[] = {vec2(0.0f, 0.0f), vec2(0.0f, 20.0f), vec2(20.0f, 0.0f), vec2(20.0f, 20.0f)};
    GLuint indices[] = {0, 1, 2, 1, 3, 2};
    vec3 colors[] = {100.f, 0.f, 0.f};
    int numVert = 4, numInd = 6;
    Model* floorModel = LoadDataToModel(vertices, vertex_normals, tex_coords, colors, indices, numVert, numInd);

    auto floor = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(floor,
                              Transform{.translation = T(bottomLeftVertex),
                                        .rotation = Ry(0)});  // we define the floor's origin at the bottom left corner
    gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = textureType});
  }
};

int main() {
  MapGenerator mapGenerator(100, 100);
  std::vector<NodePtr> map = mapGenerator.calculateMap(1, 25, 25);
  // printf(map.size() > 0 ? "Map created successfully with %lu rooms\n" : "Map creation failed\n", map.size());

  return 0;
}
