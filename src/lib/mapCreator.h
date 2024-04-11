#ifndef MAPCREATOR_H
#define MAPCREATOR_H

#include <memory>
#include <queue>
#include <vector>

#include "VectorUtils4.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"

class Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::weak_ptr<Node> WeakNodePtr;

class Node : public std::enable_shared_from_this<Node>, public Transform, public Renderable {
 public:
  std::vector<NodePtr> children;
  vec2 bottomLeftCorner, topLeftCorner, bottomRightCorner, topRightCorner;
  int treeLayerIndex;
  WeakNodePtr parent;

  Node() : treeLayerIndex(0){};  // default constructor
  Node(const vec2& bottomLeft, const vec2& topRight, int index);

  void addChild(std::shared_ptr<Node> child);
  int getWidth();
  int getHeight();
};

enum Orientation { HORIZONTAL, VERTICAL };

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

std::vector<NodePtr> findLeafNodes(const NodePtr& rootNode);

class RoomGenerator {
 public:
  RoomGenerator(){};  // default constructor

  std::vector<NodePtr> generateRooms(std::vector<NodePtr> roomSpaces, float pointModifier = 0.1);
};

class MapGenerator {
 public:
  std::vector<NodePtr> allNodesCollection;
  int mapWidth, mapHeight;

  MapGenerator(int mapWidth, int mapHeight);

  std::vector<NodePtr> calculateMap(int maxIterations, int minRoomWidth, int minRoomHeight);
};

class MapCreator {
 public:
  int mapWidth, mapHeight;
  int maxIterations, minRoomWidth, minRoomHeight;

  MapCreator(int mapWidth, int mapHeight, int maxIterations, int minRoomWidth, int minRoomHeight)
      : mapWidth(mapWidth),
        mapHeight(mapHeight),
        maxIterations(maxIterations),
        minRoomWidth(minRoomWidth),
        minRoomHeight(minRoomHeight){};
  void start();
  void createMap();
  void createFloorModel(vec2 origin, int width, int height, TextureType textureType = OFFICE_FLOOR);
};

#endif  // MAPGENERATOR_H
