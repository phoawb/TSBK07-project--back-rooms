#ifndef MAPCREATOR_H
#define MAPCREATOR_H

#include <memory>
#include <queue>
#include <vector>

#include "VectorUtils4.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/Enums.hpp"
#include "mapGeneration/BinarySpacePartitioner.h"
#include "mapGeneration/Node.h"

const int MAP_TALLNESS = 25;

std::vector<NodePtr> findLeafNodes(const NodePtr& rootNode);

class RoomGenerator {
 public:
  RoomGenerator(){};  // default constructor

  std::vector<NodePtr> generateRooms(std::vector<NodePtr> roomSpaces, float pointModifierBottom = 0.1,
                                     float pointModifierTop = 0.9, int offset = 1);
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

class RoomCreator {
 public:
  RoomCreator(){};  // default constructor
  void createRandPillarRoom(NodePtr room, int pillarSize = 10, int roomTallness = MAP_TALLNESS);

 private:
  bool isOccupied(std::vector<std::vector<TileType>>& grid, int x, int y);
};

#endif  // MAPGENERATOR_H
