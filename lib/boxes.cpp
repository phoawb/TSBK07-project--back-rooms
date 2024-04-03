#include "boxes.h"

#include "VectorUtils4.h"

/// @brief Create a box model
Model *getBoxModel(float width = 50.0f, float height = 50.0f,
                   float depth = 1.0f) {
  vec3 vertices[] = {// front
                     vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, width),
                     vec3(0.0f, height, 0.0f), vec3(0.0f, height, width),
                     // back
                     vec3(depth, 0.0f, 0.0f), vec3(depth, 0.0f, width),
                     vec3(depth, height, 0.0f), vec3(depth, height, width),
                     // right
                     vec3(0.0f, 0.0f, width), vec3(depth, 0.0f, width),
                     vec3(0.0f, height, width), vec3(depth, height, width),
                     // left
                     vec3(0.0f, 0.0f, 0.0f), vec3(depth, 0.0f, 0.0f),
                     vec3(0.0f, height, 0.0f), vec3(depth, height, 0.0f),
                     // top
                     vec3(0.0f, height, 0.0f), vec3(0.0f, height, width),
                     vec3(depth, height, 0.0f), vec3(depth, height, width),
                     // bottom
                     vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, width),
                     vec3(depth, 0.0f, 0.0f), vec3(depth, 0.0f, width)};

  vec3 vertex_normals[] = {// front
                           vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
                           vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
                           // back
                           vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
                           vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
                           // right
                           vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
                           vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
                           // left
                           vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f),
                           vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f),
                           // top
                           vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                           vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                           // bottom
                           vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f),
                           vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)};

  vec2 tex_coords[] = {
      // Front face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
      // Back face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
      // Right face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
      // Left face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
      // Top face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f),
      // Bottom face
      vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f)};

  GLuint indices[] = {// front
                      0, 1, 2, 1, 3, 2,
                      // back
                      4, 6, 5, 5, 6, 7,
                      // right
                      8, 9, 11, 8, 11, 10,
                      // left
                      15, 13, 12, 12, 14, 15,
                      // top
                      16, 17, 18, 17, 19, 18,
                      // bottom
                      20, 22, 21, 21, 22, 23};

  vec3 colors[] = {(100.f, 0.f, 0.f)};
  int numVert = 24, numInd = 36;
  Model *model = LoadDataToModel(vertices, vertex_normals, tex_coords, colors,
                                 indices, numVert, numInd);
  return model;
}