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

  void addChild(Node* child) { children.push_back(*child); }
  void removeChild(Node* child) {
    for (int i = 0; i < (int)children.size(); i++) {
      if (&children[i] == child) {
        children.erase(children.begin() + i);
        break;
      }
    }
  }
  Node(Node* parentNode) : children(), visited(false), parent(parentNode) {
    if (parentNode != nullptr) {
      treeLayerIndex = parentNode->treeLayerIndex + 1;
    } else {
      treeLayerIndex = 0;
    }
  }
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
