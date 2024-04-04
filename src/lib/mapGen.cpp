#include "mapGen.h"

#include <stdio.h>

const int MAP_SIZE = 100;
const int TILE_SIZE = 2;

std::vector<std::vector<Tile>> initGrid() {
  printf("Initializing grid\n");
  std::vector<std::vector<Tile>> grid(MAP_SIZE, std::vector<Tile>(MAP_SIZE));
  // for (int i = 0; i < MAP_SIZE; i++)
  //     for (int j = 0; j < MAP_SIZE; j++) grid[i][j] = Tile();
  return grid;
};

/* The idea for the rooms is that they follow screen coordinates for the map, and they are placed with respect to their
 * top left corner. */

void createMapBounds(std::vector<std::vector<Tile>> &grid) {
  printf("Creating map bounds\n");
  for (int i = 0; i < MAP_SIZE; i++) {
    grid[i][0] = grid[i][MAP_SIZE - 1] = Tile::WALL;
    grid[0][i] = grid[MAP_SIZE - 1][i] = Tile::WALL;
  }
}

void createStartRoom(std::vector<std::vector<Tile>> &grid, vec2 pos) {
  printf("Creating start room\n");
  int width = 40;
  int height = 20;
  for (int x = pos.x; x < pos.x + width; x++) grid[x][0] = grid[x][height - 1] = Tile::WALL;
  for (int y = pos.y; y < pos.y + height; y++) grid[0][y] = Tile::WALL;
};

BoxProps grid2BoxProps(std::vector<std::vector<Tile>> grid) {
  printf("Converting grid to box props\n");
  BoxProps props;
  props.numBoxes = 0;
  for (int y = 0; y < MAP_SIZE; y++) {
    for (int x = 0; x < MAP_SIZE; x++) {
      if (grid[x][y].type == Tile::WALL) {
        props.numBoxes++;
        GLfloat x_pos = MAP_SIZE - TILE_SIZE * y;
        GLfloat z_pos = TILE_SIZE * x - MAP_SIZE;
        props.Positions.push_back({x_pos, 0.0, z_pos});
        props.Rotations.push_back(0);
        props.Dimensions.push_back({TILE_SIZE, TILE_SIZE, TILE_SIZE});
        props.TexUnits.push_back(1);
      }
    }
  }
  return props;
}

BoxProps genWalls() {
  printf("Generating walls\n");
  BoxProps props;

  props.numBoxes = 1;
  props.Positions = {{100, 0.0, -100}};
  props.Rotations = {0};
  props.Dimensions = {{100, 50, 2}};
  props.TexUnits = {1};

  std::vector<std::vector<Tile>> grid = initGrid();
  createStartRoom(grid, {0, 0});
  props = grid2BoxProps(grid);

  printf("Generated %d walls\n", props.numBoxes);
  printf("First wall at (%f, %f, %f)\n", props.Positions[0].x, props.Positions[0].y, props.Positions[0].z);

  /*   props.numBoxes = 4;
    props.Positions = {{-50, 0.0, -50}, {-50, 0.0, 50}, {-50, 0.0, -50}, {-50, 50, -50}};
    props.Rotations = {0, M_PI_2, M_PI_2, 0};
    props.Dimensions = {{100, 50, 2}, {100, 50, 2}, {100, 50, 2}, {100, 2, 100}};
    props.TexUnits = {1, 1, 1, 1}; */
  return props;
};
