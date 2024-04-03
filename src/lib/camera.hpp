#pragma once
#include "VectorUtils4.h"

vec3 cameraDirection(float theta, float phi) { return (cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi)); }
