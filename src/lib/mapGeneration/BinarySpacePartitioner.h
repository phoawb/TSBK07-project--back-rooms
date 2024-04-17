#pragma once
#ifndef BINARYSPACEPARTITIONER_H
#define BINARYSPACEPARTITIONER_H
#include "Node.h"

enum Orientation { HORIZONTAL, VERTICAL, NONE };

class Line {
 public:
  Orientation orientation;
  vec2 coordinates;
  Line(Orientation orientation, vec2 coordinates);
};

class BinarySpacePartitioner {
 public:
  NodePtr rootNode;

  BinarySpacePartitioner(int mapWidth, int mapHeight);

  vec2 getCoordinatesForOrientation(Orientation orientation, vec2 bottomLeftCorner, vec2 topRightCorner,
                                    int minRoomWidth, int minRoomHeight);
  Line GetLineDividingSpace(vec2 bottomLeftCorner, vec2 topRightCorner, int minRoomWidth, int minRoomHeight);
  void splitTheSpace(NodePtr currentNode, int minRoomWidth, int minRoomHeight);
  std::vector<NodePtr> collectAllNodesIteratively(NodePtr inputRootNode);
  std::vector<NodePtr> prepareNodesCollection(int maxIterations, int minRoomWidth, int minRoomHeight);
};

#endif  // BINARYSPACEPARTITIONER_H