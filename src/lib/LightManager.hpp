#pragma once
#include "VectorUtils4.h"
#include "vector"

class LightManager {
 public:
  std::vector<vec3> lightSourcesColors;
  std::vector<GLint> isDirectional;
  std::vector<vec3> lightSourcesDirectionsPositions;
  int getCount() { return lightSourcesColors.size(); }
  void placeLight(vec3 lightPos, vec3 lightColor, bool isDirectional = false) {
    this->lightSourcesColors.push_back(lightColor);
    this->lightSourcesDirectionsPositions.push_back(lightPos);
    GLint isDir = isDirectional ? 1 : 0;
    this->isDirectional.push_back(isDir);
  }
};
