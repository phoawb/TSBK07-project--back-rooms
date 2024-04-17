#pragma once
#include "Node.h"
#include "vector"

std::vector<NodePtr> findLeafNodes(const NodePtr& rootNode);

class MapGenerator {
 public:
  std::vector<NodePtr> allNodesCollection;
  int mapWidth, mapHeight;

  MapGenerator(int mapWidth, int mapHeight);

  std::vector<NodePtr> calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight);
};