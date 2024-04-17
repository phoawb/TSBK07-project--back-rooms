#include "roomGenerator.h"

#include "mapUtils.h"

vec2 generateBottomLeftCornerBetween(vec2 boundaryLeftPoint, vec2 boundaryRightPoint, float pointModifier, int offset) {
  int minX = boundaryLeftPoint.x + offset;
  int maxX = boundaryRightPoint.x - offset;
  int minY = boundaryLeftPoint.y + offset;
  int maxY = boundaryRightPoint.y - offset;

  int xModifier = (maxX - minX) * pointModifier;
  int yModifier = (maxY - minY) * pointModifier;
  return vec2(randRange(minX, minX + xModifier), randRange(minY, minY + yModifier));
};

vec2 generateTopRightCornerBetween(vec2 boundaryLeftPoint, vec2 boundaryRightPoint, float pointModifier, int offset) {
  int minX = boundaryLeftPoint.x + offset;
  int maxX = boundaryRightPoint.x - offset;
  int minY = boundaryLeftPoint.y + offset;
  int maxY = boundaryRightPoint.y - offset;

  int xModifier = (maxX - minX) * pointModifier;
  int yModifier = (maxY - minY) * pointModifier;
  return vec2(randRange(minX + xModifier, maxX), randRange(minY + yModifier, maxY));
};

std::vector<NodePtr> RoomGenerator::generateRooms(std::vector<NodePtr> roomSpaces, float pointModifierBottom,
                                                  float pointModifierTop, int offset) {
  std::vector<NodePtr> rooms;
  for (auto roomSpace : roomSpaces) {
    vec2 newBottomLeftCorner = generateBottomLeftCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner,
                                                               pointModifierBottom, offset);
    vec2 newTopRightCorner =
        generateTopRightCornerBetween(roomSpace->bottomLeftCorner, roomSpace->topRightCorner, pointModifierTop, offset);
    roomSpace->bottomLeftCorner = newBottomLeftCorner;
    roomSpace->topRightCorner = newTopRightCorner;
    roomSpace->bottomRightCorner = vec2(newTopRightCorner.x, newBottomLeftCorner.y);
    roomSpace->topLeftCorner = vec2(newBottomLeftCorner.x, newTopRightCorner.y);
    rooms.push_back(roomSpace);
  }
  return roomSpaces;
};
