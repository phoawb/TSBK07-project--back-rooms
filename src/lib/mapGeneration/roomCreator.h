#pragma once
#include "Node.h"
#include "core/Constants.hpp"

class RoomCreator {
 public:
  RoomCreator(){};  // default constructor
  void createRandPillarRoom(NodePtr room, int pillarSize = 10, int roomTallness = MAP_TALLNESS);

 private:
  bool isOccupied(std::vector<std::vector<TileType>>& grid, int x, int y);
};

void createPillar(GLfloat world_x, GLfloat world_z, int pillarSize, int roomTallness);
void createCeiling(int width, int height, float thickness);
void createFloor(int width, int height, float thickness);