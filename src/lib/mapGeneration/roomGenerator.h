#pragma once
#include "Node.h"
#include "vector"

class RoomGenerator {
 public:
  RoomGenerator(){};  // default constructor

  std::vector<NodePtr> generateRooms(std::vector<NodePtr> roomSpaces, float pointModifierBottom = 0.1,
                                     float pointModifierTop = 0.9, int offset = 1);
};