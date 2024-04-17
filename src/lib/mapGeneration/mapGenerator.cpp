#include "mapGenerator.h"

#include "BinarySpacePartitioner.h"
#include "queue"
#include "roomGenerator.h"
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

MapGenerator::MapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight){};

std::vector<NodePtr> MapGenerator::calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight) {
  BinarySpacePartitioner bsp(mapWidth, mapHeight);
  allNodesCollection = bsp.prepareNodesCollection(maxIterations, minRoomWidth, minRoomHeight);
  std::vector<NodePtr> roomSpaces = findLeafNodes(bsp.rootNode);
  RoomGenerator roomGenerator = RoomGenerator();
  std::vector<NodePtr> rooms = roomGenerator.generateRooms(roomSpaces);

  return rooms;
};