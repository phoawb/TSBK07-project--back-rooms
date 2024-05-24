#pragma once
#include "Node.h"
#include "core/Constants.hpp"

class RoomCreator {
 public:
  RoomCreator(){};  // default constructor
  void createRandPillarRoom(NodePtr room, int pillarSize = 10, int roomTallness = MAP_TALLNESS);
  void createOfficeRoom(NodePtr room, int roomTallness = MAP_TALLNESS, int gateSize = 10);

 private:
  bool isOccupied(std::vector<std::vector<TileType>>& grid, int x, int y, TileType type);
  void placeTileTypes(std::vector<std::vector<TileType>>& grid, int maxTiles, int maxFailedTries, TileType type);
};

void createPillar(GLfloat world_x, GLfloat world_z, int pillarSize, int roomTallness);
void createMapCeiling(int mapWidth, int mapHeight, float thickness, int tileScale = 10);
void createCeiling(GLfloat world_x, GLfloat world_z, int width, int height, float thickness, int tileScale,
                   int textureScale);
void createMapFloor(int mapWidth, int mapHeight, float thickness, int tileScale = 10);
void createFloor(GLfloat world_x, GLfloat world_z, int width, int height, float thickness, int tileScale,
                 int textureScale = 1);
void createLight(GLfloat world_x, GLfloat world_z, int width = 10, int height = 10, float thickness = 0.3);
void createMapWalls(int mapWidth, int mapheight, float wallThickness);