// #include "VectorUtils4.h"
#include "algorithm"
#include "memory"
#include "queue"
#include "random"
#include "stdio.h"
#include "vector"

// vec2 is mostly used for texture cordinates, so I havn't bothered defining any operations for it
typedef struct vec2 {
  int x, y;
  vec2() {}
  vec2(int x2, int y2) : x(x2), y(y2) {}
  bool operator==(const vec2& other) const { return x == other.x && y == other.y; }
} vec2, *vec2_ptr;

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

class MapCreator {
 public:
  std::vector<NodePtr> allNodesCollection;
  int mapWidth, mapHeight;

  MapCreator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

  std::vector<NodePtr> calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
    BinarySpacePartitioner bsp(mapWidth, mapHeight);
    allNodesCollection = bsp.prepareNodesCollection(maxIterations, minRoomWidth, minRoomHeight);
    std::vector<NodePtr> roomSpaces = findLeafNodes(bsp.rootNode);
    return allNodesCollection;
  };
};

int main() {
  MapCreator mapCreator(100, 100);
  std::vector<NodePtr> map = mapCreator.calculateMap(10, 25, 25);
  // printf(map.size() > 0 ? "Map created successfully with %lu rooms\n" : "Map creation failed\n", map.size());

  return 0;
}
