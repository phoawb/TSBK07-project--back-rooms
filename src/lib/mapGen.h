// mapGen.h
#ifndef MAPGEN_H
#define MAPGEN_H

#include <vector>

#include "GL_utilities.h"
#include "VectorUtils4.h"

// Tile enum as used in the CPP file

class Tile {
 public:
  enum Type { EMPTY, WALL, PILLAR_ROOM, OFFICE_ROOM, HALLWAY, START_ROOM, END_ROOM };
  Tile(Type inputType = EMPTY) : type(inputType){};
  bool rotate = false;
  bool isStartEndRoom = false;
  Type type = Type::EMPTY;
};

// BoxProps struct as used in the CPP file
struct BoxProps {
  int numBoxes;
  std::vector<vec3> Positions;
  std::vector<float> Rotations;
  std::vector<vec3> Dimensions;
  std::vector<GLuint> TexUnits;
};

// Function declarations
std::vector<std::vector<Tile>> initGrid();
void createMapBounds(BoxProps& BoxProps);
void createStartEndRoom(std::vector<std::vector<Tile>>& grid, vec2 pos, bool iseEndRoom = false);
bool isOccupied(std::vector<std::vector<Tile>>& grid, int x, int y);
void createRandomWalls(std::vector<std::vector<Tile>>& grid, int noOfWalls);
void grid2BoxProps(std::vector<std::vector<Tile>> grid, BoxProps& props);
void createCeiling(BoxProps& props);
BoxProps genWalls();

#endif  // MAPGEN_H
