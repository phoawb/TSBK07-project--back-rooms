#pragma once
#include "VectorUtils4.h"
#include "vector"

// TODO: remove this
struct LightManager {
  std::vector<vec3> lightSourcesColors;
  std::vector<GLint> isDirectional;
  std::vector<vec3> lightSourcesDirectionsPositions;
  int getCount() { return lightSourcesColors.size(); }
};
