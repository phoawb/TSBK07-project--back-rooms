#include "ground.h"

Model *getGroundModel(float groundSize) {
  vec3 vertices[] = {vec3(-groundSize, 0.0f, -groundSize), vec3(-groundSize, 0.0f, groundSize),
                     vec3(groundSize, -0.0f, -groundSize), vec3(groundSize, -0.0f, groundSize)};

  vec3 vertex_normals[] = {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                           vec3(0.0f, 1.0f, 0.0f)};

  vec2 tex_coords[] = {vec2(0.0f, 0.0f), vec2(0.0f, 20.0f), vec2(20.0f, 0.0f), vec2(20.0f, 20.0f)};
  GLuint indices[] = {0, 1, 2, 1, 3, 2};
  vec3 colors[] = {100.f, 0.f, 0.f};
  int numVert = 4, numInd = 6;
  Model *model = LoadDataToModel(vertices, vertex_normals, tex_coords, colors, indices, numVert, numInd);
  return model;
}