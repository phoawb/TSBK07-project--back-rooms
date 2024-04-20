#pragma once

enum ShaderType { NO_SHADE, TERRAIN, LAMP };

enum TextureType {
  OFFICE_WALL,
  GRASS,
  SKYBOX_SKY,
  SKYBOX_CITY,
  DIRT,
  OFFICE_FLOOR,
  CONCRETE,
  OFFICE_CEILING,
  WHITE,
  OFFWHITE,
  BLACK,
  CARTON_TEX
};

enum ModelType { SKYBOX, SPHERE, CHAIR, CARTON };

enum TileType { EMPTY, PILLAR, LIGHT, WALL };
enum RoomTypes { RAND_PILLARS, OFFICE_ROOM };
const int NO_OF_ROOM_TYPES = 2;
