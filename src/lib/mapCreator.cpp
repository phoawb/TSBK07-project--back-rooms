#include "VectorUtils4.h"
#include "vector"

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

  Node(Node* parentNode) : children(), visited(false), parent(parentNode) {}
};

class RoomNode {
 private:
  std::vector<RoomNode> children;
};

class MapCreator {
 public:
  int width, height;
  int minRoomWidth, minRoomHeight;
  int maxIterations;
  int corridorWidth;

  MapCreator(int mapWidth, int mapHeight) : width(mapWidth), height(mapHeight) {
    minRoomWidth = 5;
    minRoomHeight = 5;
    maxIterations = 1000;
    corridorWidth = 1;
  };

  void createMap(){

  };
};
