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
  bool isOccupied() { return type != Type::EMPTY; }
  bool rotate = false;
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
void createMapBounds(std::vector<std::vector<Tile>> &grid);
void createStartRoom(std::vector<std::vector<Tile>> &grid, vec2 pos);
BoxProps grid2BoxProps(std::vector<std::vector<Tile>> grid);
BoxProps genWalls();

#endif  // MAPGEN_H
