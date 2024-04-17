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