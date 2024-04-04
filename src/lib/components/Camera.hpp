#pragma once

#include "VectorUtils4.h"

struct Camera {
  mat4 projectionTransform;
  mat4 matrix;
  float theta;
  float phi;
  vec3 lookAt;
  vec3 cameraUp;

  /*
    static mat4 MakeProjectionTransform(float fov, float nearClip, float farClip, unsigned int viewWidth,
                                        unsigned int viewHeight) {
      float zClipBias0 = (farClip + nearClip) / (farClip - nearClip);

      float zClipBias1 = (2.0f * farClip * nearClip) / (farClip - nearClip);

      float zoomX = 1.0f / tanf((fov / 2.0f) * (M_PI / 180.0f));
      float zoomY = (zoomX * static_cast<float>(viewWidth)) / static_cast<float>(viewHeight);

      mat4 transform;
      transform.m[0][0] = zoomX;
      transform.m[1][1] = zoomY;
      transform.m[2][2] = -zClipBias0;
      transform.m[3][2] = -1;
      transform.m[2][3] = -zClipBias1;

   return transform;
  }
      */
};