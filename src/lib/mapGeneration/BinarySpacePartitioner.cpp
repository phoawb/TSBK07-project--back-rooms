#include "BinarySpacePartitioner.h"

#include "mapUtils.h"
#include "queue"

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