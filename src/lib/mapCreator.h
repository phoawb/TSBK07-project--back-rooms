#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "VectorUtils4.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/Enums.hpp"
#include "mapGeneration/BinarySpacePartitioner.h"
#include "mapGeneration/Node.h"

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
