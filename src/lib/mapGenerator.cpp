#include <random>

#include "VectorUtils4.h"
#include "algorithm"
#include "queue"
#include "vector"

int randRange(int min, int max) {
  static std::random_device rd;                     // Obtain a random number from hardware
  static std::mt19937 gen(rd());                    // Seed the generator
  std::uniform_int_distribution<> distr(min, max);  // Define the range

  return distr(gen);
}

class Node {
 public:
  std::vector<Node> children;
  bool visited;
  vec2 bottomLeftCorner;
  vec2 topLeftCorner;
  vec2 bottomRightCorner;
  vec2 topRightCorner;
  int treeLayerIndex;
  Node* parent;

  void addChild(Node child) { children.push_back(child); };
  void RemoveChild(Node child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());  // potential memory leak?
  };

  Node(Node* parentNode) : parent(parentNode) {
    this->children = std::vector<Node>();
    if (parentNode != NULL) parentNode->addChild(*this);
  }
};

class RoomNode : public Node {
 public:
  RoomNode(vec2 bottomLeftCorner, vec2 topRightCorner, Node parentNode, int index) : Node(parentNode) {
    this->bottomLeftCorner = bottomLeftCorner;
    this->topRightCorner = topRightCorner;
    this->bottomRightCorner = vec2(topRightCorner.x, bottomLeftCorner.y);
    this->topLeftCorner = vec2(bottomLeftCorner.x, topRightCorner.y);
    this->treeLayerIndex = index;
  }

  RoomNode() : Node(NULL) {
    this->bottomLeftCorner = vec2(0, 0);
    this->topRightCorner = vec2(0, 0);
    this->bottomRightCorner = vec2(0, 0);
    this->topLeftCorner = vec2(0, 0);
    this->treeLayerIndex = 0;
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
  RoomNode rootNode;

  BinarySpacePartitioner(int mapWidth, int mapHeight) {
    rootNode = RoomNode(vec2(0, 0), vec2(mapWidth, mapHeight), NULL, 0);
  };

  vec2 getCoordinatesForOrientation(Orientation orientation, vec2 bottomLeftCorner, vec2 topRightCorner,
                                    int minRoomWidth, int minRoomHeight) {
    if (orientation == HORIZONTAL)
      return vec2(0, randRange((bottomLeftCorner.y + minRoomHeight), (topRightCorner.y - minRoomHeight)));
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

    return Line(orientation, getCoordinatesForOrientation(orientation, bottomLeftCorner, topRightCorner, minRoomWidth,
                                                          minRoomHeight));
  }

  void addNewNodeToCollections(std::vector<RoomNode>& listToReturn, std::queue<RoomNode>& graph, RoomNode node) {
    listToReturn.push_back(node);
    graph.push(node);
  }

  void splitTheSpace(RoomNode currentNode, std::vector<RoomNode>& listToReturn, int minRoomWidth, int minRoomHeight,
                     std::queue<RoomNode>& graph) {
    Line line =
        GetLineDividingSpace(currentNode.bottomLeftCorner, currentNode.topRightCorner, minRoomWidth, minRoomHeight);
    RoomNode node1, node2;
    if (line.orientation == HORIZONTAL) {
      node1 = RoomNode(currentNode.bottomLeftCorner, vec2(currentNode.topRightCorner.x, line.coordinates.y),
                       currentNode, currentNode.treeLayerIndex + 1);
      node2 = RoomNode(vec2(currentNode.bottomLeftCorner.x, line.coordinates.y), currentNode.topRightCorner,
                       currentNode, currentNode.treeLayerIndex + 1);
    } else {
      node1 = RoomNode(currentNode.bottomLeftCorner, vec2(line.coordinates.x, currentNode.topRightCorner.y),
                       currentNode, currentNode.treeLayerIndex + 1);
      node2 = RoomNode(vec2(line.coordinates.x, currentNode.bottomLeftCorner.y), currentNode.topRightCorner,
                       currentNode, currentNode.treeLayerIndex + 1);
    }
    addNewNodeToCollections(listToReturn, graph, node1);
    addNewNodeToCollections(listToReturn, graph, node2);
  }

  std::vector<RoomNode> prepareNodesCollection(int maxIterations, int minRoomWidth, int minRoomHeight) {
    std::queue<RoomNode> graph = std::queue<RoomNode>();
    std::vector<RoomNode> listToReturn = std::vector<RoomNode>();
    graph.push(rootNode);
    listToReturn.push_back(rootNode);
    int iterations = 0;
    while (iterations < maxIterations && !graph.empty()) {
      iterations++;
      RoomNode currentNode = graph.front();
      graph.pop();
      if (currentNode.getWidth() >= minRoomWidth * 2 || currentNode.getHeight() >= minRoomHeight * 2) {
        splitTheSpace(currentNode, listToReturn, minRoomWidth, minRoomHeight, graph);
      }
    }
    return listToReturn;
  };
};

class MapCreator {
 public:
  std::vector<RoomNode> allNodesCollection = std::vector<RoomNode>();
  int mapWidth, mapHeight;

  MapCreator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

  std::vector<Node> calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
    BinarySpacePartitioner bsp(mapWidth, mapHeight);
  };
};
