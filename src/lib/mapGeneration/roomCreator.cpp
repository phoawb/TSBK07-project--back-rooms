#include "roomCreator.h"

#include "boxes.h"
#include "components/AABB.hpp"
#include "components/Light.hpp"
#include "components/RigidBody.hpp"
#include "core/Coordinator.hpp"
#include "stdio.h"

extern Coordinator gCoordinator;

void createPillar(GLfloat world_x, GLfloat world_z, int pillarSize, int roomTallness) {
  Model* pillarModel = getBoxModel(pillarSize, roomTallness, pillarSize, 1);
  auto pillar = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(pillar, Transform{.translation = T(world_x, 0, world_z)});
  gCoordinator.AddComponent(pillar, Renderable{.model = pillarModel, .shader = TERRAIN, .texture = OFFICE_WALL});
  gCoordinator.AddComponent(pillar, AABB{.dimensions = vec3(pillarSize, roomTallness, pillarSize)});
  gCoordinator.AddComponent(pillar, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
  // moldings for the pillars (bot and top)
  for (float m = 0.f; m <= MAP_TALLNESS - MOLDING_HEIGHT; m += MAP_TALLNESS - MOLDING_HEIGHT) {
    Model* moldingModel =
        getBoxModel(pillarSize + MOLDING_DEPTH * 2, MOLDING_HEIGHT, pillarSize + MOLDING_DEPTH * 2, 1);
    auto molding = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(molding,
                              Transform{.translation = T(world_x - MOLDING_DEPTH, m, world_z - MOLDING_DEPTH)});
    gCoordinator.AddComponent(molding, Renderable{.model = moldingModel, .shader = TERRAIN, .texture = OFFWHITE});
    gCoordinator.AddComponent(molding, AABB{.dimensions = vec3(pillarSize + MOLDING_DEPTH * 2, MOLDING_HEIGHT,
                                                               pillarSize + MOLDING_DEPTH * 2)});
    gCoordinator.AddComponent(molding, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
  }
}

void createMapCeiling(int mapWidth, int mapHeight, float thickness, int tileScale) {
  int ceilingWidth = mapWidth / tileScale;
  int ceilingHeight = mapHeight / tileScale;

  for (int x = 0; x < tileScale; x++) {
    for (int z = 0; z < tileScale; z++) {
      createCeiling(x * ceilingWidth, z * ceilingHeight, ceilingWidth, ceilingHeight, thickness, tileScale, 3);
    }

    /*   Model* ceilingModel = getBoxModel(width, thickness, height, 25);
      auto ceiling = gCoordinator.CreateEntity();
      gCoordinator.AddComponent(ceiling, Transform{.translation = T({0.f, MAP_TALLNESS, 0.f})});
      gCoordinator.AddComponent(ceiling, Renderable{.model = ceilingModel, .shader = TERRAIN, .texture =
      OFFICE_CEILING}); gCoordinator.AddComponent(ceiling, AABB{.dimensions = vec3(width, thickness, height)});
      gCoordinator.AddComponent(ceiling, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)}); */
  }
}

void createCeiling(GLfloat world_x, GLfloat world_z, int width, int height, float thickness, int tileScale,
                   int textureScale) {
  Model* ceilingModel = getBoxModel(height, thickness, width, textureScale);
  auto ceiling = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(ceiling, Transform{.translation = T({world_x, MAP_TALLNESS, world_z})});
  gCoordinator.AddComponent(ceiling, Renderable{.model = ceilingModel, .shader = TERRAIN, .texture = OFFICE_CEILING});
  gCoordinator.AddComponent(ceiling, AABB{.dimensions = vec3(height, thickness, width)});
  gCoordinator.AddComponent(ceiling, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
};

void createMapFloor(int mapWidth, int mapHeight, float thickness, int tileScale) {
  int floorWidth = mapWidth / tileScale;
  int floorHeight = mapHeight / tileScale;

  for (int x = 0; x < tileScale; x++) {
    for (int z = 0; z < tileScale; z++) {
      createFloor(x * floorWidth, z * floorHeight, floorWidth, floorHeight, thickness, tileScale);
    }
  }
}

void createFloor(GLfloat world_x, GLfloat world_z, int width, int height, float thickness, int tileScale,
                 int textureScale) {
  Model* floorModel = getBoxModel(height, thickness, width, textureScale);
  auto floor = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(floor, Transform{.translation = T({world_x, -thickness, world_z})});
  gCoordinator.AddComponent(floor, Renderable{.model = floorModel, .shader = TERRAIN, .texture = OFFICE_FLOOR});
  gCoordinator.AddComponent(floor, AABB{.dimensions = vec3(height, thickness, width)});
  gCoordinator.AddComponent(floor, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
}

void createLight(GLfloat world_x, GLfloat world_z, int width, int height, float thickness) {
  Model* lampModel = getBoxModel(width, thickness, height, 1);
  auto lamp = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(lamp, Transform{.translation = T(world_x, MAP_TALLNESS - thickness, world_z)});
  gCoordinator.AddComponent(lamp, Renderable{.model = lampModel, .shader = LAMP, .texture = WHITE});
  gCoordinator.AddComponent(lamp, AABB{.dimensions = vec3(width, thickness, height)});
  gCoordinator.AddComponent(lamp, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});

  // actual light
  auto lightEntity = gCoordinator.CreateEntity();
  gCoordinator.AddComponent(lightEntity, Transform{.translation = T(world_x + width / 2.f, MAP_TALLNESS - thickness,
                                                                    world_z + height / 2.f)});
  gCoordinator.AddComponent(lightEntity, Light{.color = vec3(1, 1, 1), .shader = TERRAIN});
}

bool RoomCreator::isOccupied(std::vector<std::vector<TileType>>& grid, int x, int y, TileType type) {
  if (grid[x][y] != TileType::EMPTY) return true;
  std::vector<vec2> directions = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                  {0, 2}, {0, -2}, {2, 0},  {-2, 0},  {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
                                  {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
  for (auto& direction : directions) {
    int newX = x + direction.x;
    int newY = y + direction.y;
    // check if out of bounds
    if (newX < 0 || newX >= (int)grid.size() || newY < 0 || newY >= (int)grid[0].size()) continue;
    // check if occupied
    if (grid[newX][newY] == type || grid[newX][newY] == WALL) return true;
  }
  return false;
}

int TOTAL_LIGHTS_ROOM = 0;
void RoomCreator::createRandPillarRoom(NodePtr room, int pillarSize, int roomTallness) {
  int roomWidth = room->getWidth();
  int roomHeight = room->getHeight();
  int roomX = room->bottomLeftCorner.x;
  int roomY = room->bottomLeftCorner.y;
  int gridWidth = roomWidth / pillarSize;
  int gridHeight = roomHeight / pillarSize;
  std::vector<std::vector<TileType>> grid(gridWidth, std::vector<TileType>(gridHeight, EMPTY));

  // set seed
  srand(time(NULL));

  int maxPillars = gridWidth * gridHeight;
  int maxLights = 5;
  int maxFailedTries = 1000;
  placeTileTypes(grid, maxPillars, maxFailedTries, PILLAR);
  placeTileTypes(grid, maxLights, maxFailedTries, LIGHT);
  // printf("Total lights: %d\n", TOTAL_LIGHTS_ROOM);

  for (int y = 0; y < gridHeight; y++) {
    for (int x = 0; x < gridWidth; x++) {
      GLfloat world_x = roomY + y * pillarSize;
      GLfloat world_z = roomX + x * pillarSize;
      if (grid[x][y] == PILLAR) {
        createPillar(world_x, world_z, pillarSize, roomTallness);
      } else if (grid[x][y] == LIGHT) {
        createLight(world_x, world_z);
      }
    }
  }
};

void createWall(worldVec2 coordinates, float width, float height, float depth, float textureScale) {
  Model* wallModel = getBoxModel(width, height, depth, textureScale);
  auto wall = gCoordinator.CreateEntity();
  // printf("Creating wall at %f, %f\n", coordinates.x, coordinates.z);
  gCoordinator.AddComponent(wall, Transform{.translation = T(coordinates.x, 0, coordinates.z)});
  gCoordinator.AddComponent(wall, Renderable{.model = wallModel, .shader = TERRAIN, .texture = OFFICE_WALL});
  gCoordinator.AddComponent(wall, AABB{.dimensions = vec3(depth, height, width)});  // width and depth are flipped :(
  gCoordinator.AddComponent(wall, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});

  Model* moldingModel =
      getBoxModel(width + MOLDING_DEPTH * 2, MOLDING_HEIGHT - 0.5, depth + MOLDING_DEPTH * 2, textureScale);
  auto molding = gCoordinator.CreateEntity();
  // printf("Creating wall at %f, %f\n", coordinates.x, coordinates.z);
  gCoordinator.AddComponent(
      molding, Transform{.translation = T(coordinates.x - MOLDING_DEPTH, 0, coordinates.z - MOLDING_DEPTH)});
  gCoordinator.AddComponent(molding, Renderable{.model = moldingModel, .shader = TERRAIN, .texture = OFFWHITE});
  gCoordinator.AddComponent(molding, RigidBody{.isStatic = true, .velocity = vec3(0), .acceleration = vec3(0)});
}

void createMapWalls(int mapWidth, int mapheight, float wallThickness) {
  int individualWallLength{20};
  int noOfWalls = mapWidth / individualWallLength;
  for (int i = 0; i < noOfWalls; i++) {
    createWall(worldVec2(i * individualWallLength, 0), individualWallLength, MAP_TALLNESS, wallThickness, 1.f);
    createWall(worldVec2(i * individualWallLength, mapheight), individualWallLength, MAP_TALLNESS, wallThickness, 1.f);
    createWall(worldVec2(0, i * individualWallLength), wallThickness, MAP_TALLNESS, individualWallLength, 1.f);
    createWall(worldVec2(mapWidth, i * individualWallLength), wallThickness, MAP_TALLNESS, individualWallLength, 1.f);
  }
}

void RoomCreator::createOfficeRoom(NodePtr room, int roomTallness, int gateSize) {
  // NOTE: width and height are flipped! (width is along the z-axis)
  int roomWidth = room->getWidth();
  int roomHeight = room->getHeight();
  int gridWidth = roomWidth / WALL_THICKNESS;
  int gridHeight = roomHeight / WALL_THICKNESS;
  std::vector<std::vector<TileType>> grid(gridWidth, std::vector<TileType>(gridHeight, EMPTY));
  worldVec2 roomOrigin = {room->bottomLeftCorner.x, room->bottomLeftCorner.y};
  // create the room walls with passages in the middle
  // bottom wall 1
  createWall(roomOrigin, roomWidth / 2.f - gateSize, roomTallness, WALL_THICKNESS, 1);
  // bottom wall 2
  createWall({roomOrigin.z + roomWidth / 2.f + gateSize, roomOrigin.x}, roomWidth / 2.f - gateSize, roomTallness,
             WALL_THICKNESS, 1);
  // top wall 1
  createWall({roomOrigin.z, roomOrigin.x + roomHeight - WALL_THICKNESS}, roomWidth / 2.f - gateSize, roomTallness,
             WALL_THICKNESS, 1);
  // top wall 2
  createWall({roomOrigin.z + roomWidth / 2.f + gateSize, roomOrigin.x + roomHeight - WALL_THICKNESS},
             roomWidth / 2.f - gateSize, roomTallness, WALL_THICKNESS, 1);
  // left wall 1
  createWall({roomOrigin.z, roomOrigin.x + WALL_THICKNESS}, WALL_THICKNESS, roomTallness,
             roomHeight / 2.f - gateSize - WALL_THICKNESS, 1);
  // left wall 2
  createWall({roomOrigin.z, roomOrigin.x + roomHeight / 2.f + gateSize - WALL_THICKNESS}, WALL_THICKNESS, roomTallness,
             roomHeight / 2.f - gateSize - WALL_THICKNESS, 1);
  // right wall 1
  createWall({roomOrigin.z + roomWidth - WALL_THICKNESS, roomOrigin.x + WALL_THICKNESS}, WALL_THICKNESS, roomTallness,
             roomHeight / 2.f - gateSize - WALL_THICKNESS, 1);
  // right wall 2
  createWall({roomOrigin.z + roomWidth - WALL_THICKNESS, roomOrigin.x + roomHeight / 2.f + gateSize - WALL_THICKNESS},
             WALL_THICKNESS, roomTallness, roomHeight / 2.f - gateSize - WALL_THICKNESS, 1);
  // populate the grid's sides with walls
  for (int x = 0; x < gridHeight; x++) {
    for (int z = 0; z < gridWidth; z++)
      if (z == 0 || z == gridWidth - 1 || x == 0 || x == gridHeight - 1) grid[z][x] = WALL;
  }

  int maxFailedTries = 1000;
  int maxPillars = 4;
  int maxLights = 4;
  if (roomWidth == MIN_ROOM_SIZE || roomHeight == MIN_ROOM_SIZE) maxPillars = 2, maxLights = 2;
  placeTileTypes(grid, maxPillars, maxFailedTries, PILLAR);
  placeTileTypes(grid, maxLights, maxFailedTries, LIGHT);

  for (int x = 0; x < gridHeight; x++) {
    for (int z = 0; z < gridWidth; z++) {
      GLfloat world_x = roomOrigin.x + x * WALL_THICKNESS;
      GLfloat world_z = roomOrigin.z + z * WALL_THICKNESS;
      if (grid[z][x] == PILLAR) {
        createPillar(world_x, world_z, WALL_THICKNESS, roomTallness);
      } else if (grid[z][x] == LIGHT) {
        createLight(world_x, world_z);
      }
    }
  }
}
void RoomCreator::placeTileTypes(std::vector<std::vector<TileType>>& grid, int maxTiles, int maxFailedTries,
                                 TileType type) {
  // set seed
  srand(time(NULL));

  int failedTries = 0;
  int placedTiles = 0;
  while (placedTiles < maxTiles && failedTries < maxFailedTries) {
    int x = rand() % grid.size();
    int y = rand() % grid[0].size();
    if (isOccupied(grid, x, y, type)) {
      failedTries++;
      continue;
    }
    grid[x][y] = type;
    placedTiles++;
    failedTries = 0;
  }
}