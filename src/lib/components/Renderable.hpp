#pragma once

#include "LittleOBJLoader.h"

struct Renderable {
  Model* model;
  GLuint program;
  GLuint texUnit;
};