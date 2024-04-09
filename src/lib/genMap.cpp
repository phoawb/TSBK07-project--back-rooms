#include "genMap.h"

#include <boxes.h>
#include <stdio.h>

#include "ShaderManager.hpp"
#include "components/AABB.hpp"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"
#include "core/Coordinator.hpp"

extern Coordinator gCoordinator;

const int MAP_SIZE = 100;
const int TILE_SIZE = 10;
const int WALL_THICKNESS = 2;
const int WALL_TALLNESS = 25;
const int GRID_SIZE = MAP_SIZE * 2 / TILE_SIZE;
int START_ROOM_WIDTH = 80 / TILE_SIZE;
int START_ROOM_HEIGHT = 40 / TILE_SIZE;

std::vector<std::vector<Tile>> initGrid() {
  // printf("Initializing grid\n");
  std::vector<std::vector<Tile>> grid(GRID_SIZE, std::vector<Tile>(GRID_SIZE));
  return grid;
};

/* The idea for the rooms is that they follow screen coordinates for the map, and they are placed with respect to their
 * top left corner. */

void createMapBounds(BoxProps& boxProps) {
  // printf("Creating map bounds\n");
  int boundTextureScale = 6;
  boxProps.numBoxes += 4;
  // printf("Num boxes: %d\n", boxProps.numBoxes);
  boxProps.Positions.push_back({MAP_SIZE, 0.0, -MAP_SIZE});
  boxProps.Rotations.push_back(2 * M_PI);
  boxProps.Dimensions.push_back({MAP_SIZE * 2, WALL_TALLNESS, WALL_THICKNESS});
  boxProps.TextureTypes.push_back(TextureType::OFFICE_WALL);
  boxProps.TextureScale.push_back(boundTextureScale);

  boxProps.Positions.push_back({-MAP_SIZE - WALL_THICKNESS, 0.0, -MAP_SIZE});
  boxProps.Rotations.push_back(0);
  boxProps.Dimensions.push_back({MAP_SIZE * 2, WALL_TALLNESS, WALL_THICKNESS});
  boxProps.TextureTypes.push_back(TextureType::OFFICE_WALL);
  boxProps.TextureScale.push_back(boundTextureScale);

  boxProps.Positions.push_back({-MAP_SIZE, 0.0, MAP_SIZE + WALL_THICKNESS});
  boxProps.Rotations.push_back(M_PI_2);
  boxProps.Dimensions.push_back({MAP_SIZE * 2, WALL_TALLNESS, WALL_THICKNESS});
  boxProps.TextureTypes.push_back(TextureType::OFFICE_WALL);
  boxProps.TextureScale.push_back(boundTextureScale);

  boxProps.Positions.push_back({-MAP_SIZE, 0.0, -MAP_SIZE});
  boxProps.Rotations.push_back(M_PI_2);
  boxProps.Dimensions.push_back({MAP_SIZE * 2, WALL_TALLNESS, WALL_THICKNESS});
  boxProps.TextureTypes.push_back(TextureType::OFFICE_WALL);
  boxProps.TextureScale.push_back(boundTextureScale);
}

void createStartEndRoom(std::vector<std::vector<Tile>>& grid, vec2 pos, bool iseEndRoom) {
  // printf("Creating start room\n");
  int y_compare = iseEndRoom ? pos.y : pos.y + START_ROOM_HEIGHT - 1;
  for (int x = pos.x; x < pos.x + START_ROOM_WIDTH; x++) {
    for (int y = pos.y; y < pos.y + START_ROOM_HEIGHT; y++) {
      grid[x][y].isStartEndRoom = true;
      if (y == y_compare) grid[x][y].type = Tile::WALL;
    }
  }
}

// Check if a tile is occupied by a wall or if it is adjacent to a wall
bool isOccupied(std::vector<std::vector<Tile>>& grid, int x, int y) {
  if (grid[x][y].type != Tile::EMPTY || grid[x][y].isStartEndRoom) return true;
  std::vector<vec2> directions = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                  {0, 2}, {0, -2}, {2, 0},  {-2, 0},  {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
                                  {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
  for (auto dir : directions) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    // Check if out of bounds
    if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) continue;
    // check start/end room
    if (grid[newX][newY].isStartEndRoom) return true;
    // Check if occupied
    if (grid[newX][newY].type != Tile::EMPTY) return true;
  }
  return false;
}

void createRandomWalls(std::vector<std::vector<Tile>>& grid, int noOfWalls) {
  enum WALL_TYPE { SMALL };  // , HORIZONTAL, VERTICAL, SQUARE };
  // printf("Creating random walls\n");

  // set seed
  srand(time(NULL));

  int failedTries = 0;
  int placedWalls = 0;
  while (placedWalls < noOfWalls && failedTries < 10000) {
    int x = rand() % GRID_SIZE;
    int y = rand() % GRID_SIZE;
    // // printf("Trying to place wall at (%d, %d)\n", x, y);

    if (isOccupied(grid, x, y)) {
      failedTries++;
      continue;
    }
    if (grid[x][y].isStartEndRoom) {
      // printf("This should not happen\n");
    }
    if (x >= 12 && y >= 16) {
      // printf("This should not happen\n");
      // printf("weird placement at (%d, %d)\n", x, y);
      // printf("Tile isStartEndRoom: %d\n", grid[x][y].isStartEndRoom);
    }
    // printf("Placing wall at (%d, %d)\n", x, y);
    grid[x][y].type = Tile::WALL;
    placedWalls++;
    failedTries = 0;
  }
};

void grid2BoxProps(std::vector<std::vector<Tile>> grid, BoxProps& props) {
  // printf("Converting grid to box props\n");
  for (int y = 0; y < GRID_SIZE; y++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      if (grid[x][y].type == Tile::WALL) {
        props.numBoxes++;
        GLfloat x_pos = MAP_SIZE - TILE_SIZE * y - TILE_SIZE;
        GLfloat z_pos = TILE_SIZE * x - MAP_SIZE;
        props.Positions.push_back({x_pos, 0.0, z_pos});
        props.Rotations.push_back(grid[x][y].rotate ? M_PI_2 : 0);
        GLfloat tileSize = grid[x][y].isStartEndRoom ? 2 : TILE_SIZE;
        props.Dimensions.push_back({TILE_SIZE, WALL_TALLNESS, tileSize});
        props.TextureTypes.push_back(TextureType::OFFICE_WALL);
        props.TextureScale.push_back(1);
      }
    }
  }
}

void createCeiling(BoxProps& props) {
  // printf("Creating ceiling\n");
  props.numBoxes += 1;
  props.Positions.push_back({-100.0, WALL_TALLNESS, -100.0});
  props.Rotations.push_back(0);
  props.Dimensions.push_back({MAP_SIZE * 2, WALL_THICKNESS, MAP_SIZE * 2});
  props.TextureTypes.push_back(TextureType::OFFICE_CEILING);
  props.TextureScale.push_back(5);
}

void placeWallEnteties(BoxProps& props) {
  // printf("Placing walls\n");
  for (int i = 0; i < props.numBoxes; i++) {
    auto entity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(
        entity, Transform{.translation = T(props.Positions[i].x, props.Positions[i].y, props.Positions[i].z),
                          .rotation = Ry(props.Rotations[i])});
    gCoordinator.AddComponent(entity, Renderable{.model = getBoxModel(props.Dimensions[i].x, props.Dimensions[i].y,
                                                                      props.Dimensions[i].z, props.TextureScale[i]),
                                                 .shader = TERRAIN,
                                                 .texture = props.TextureTypes[i]});
    gCoordinator.AddComponent(
        entity, AABB{.minPoint = props.Positions[i], .maxPoint = props.Positions[i] + props.Dimensions[i]});
  }
}

void createWall(int x, int y, std::vector<std::vector<Tile>>& grid) {
  grid[x][y].type = Tile::WALL;
  // grid[x][y].rotate = true;
}

void genMap() {
  // printf("Generating walls\n");
  BoxProps props;
  props.numBoxes = 0;

  std::vector<std::vector<Tile>> grid = initGrid();
  createMapBounds(props);
  createStartEndRoom(grid, {0, 0});
  createStartEndRoom(grid, {12, 16}, true);  // act as endroom for now
  createRandomWalls(grid, 40);
  // createWall(11, 10, grid);
  grid2BoxProps(grid, props);
  createCeiling(props);

  placeWallEnteties(props);

  // printf("Generated %d walls\n", props.numBoxes);
  // printf("First wall at (%f, %f, %f)\n", props.Positions[0].x, props.Positions[0].y, props.Positions[0].z);
};
