// Lab 4, terrain generation

// uses framework Cocoa
// uses framework OpenGL
#define MAIN
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "MicroGlut.h"
#include "VectorUtils4.h"

// time
GLfloat t;

mat4 cameraMatrix;

// vertex array object
Model *m, *m2, *terrainModel, *groundSphereModel;
// Reference to shader program
GLuint terrainProgram;
GLuint map, grass, dirt, conc;
TextureData ttex;  // terrain

// camera
vec3 cameraPos(60.f, 40.f, 0.f);
vec3 cameraLookAt(0.f, 10.f, 0.f);
vec3 cameraUp(0.f, 1.f, 0.f);
vec3 groundBallPos = vec3(0, 0, 0);
float theta = -M_PI_2;
float phi = 0;
bool isSlow = false;

mat4 projectionMatrix;

void loadModels() {
  groundSphereModel = LoadModelPlus("objects/groundsphere.obj");
}

/// @brief Get the height of the terrain at a specific point
/// @param tex TextureData containing the terrain data
/// @param x The x-coordinate of the point
/// @param z The z-coordinate of the point
/// @return The height of the terrain at the specified point
GLfloat coord2height(TextureData *tex, unsigned int x, unsigned int z) {
  return tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 10.0;
}

GLfloat coord2height(TextureData *tex, double x, double z) {
  unsigned int fX = (int)floor(x);
  unsigned int cX = (int)ceil(x);
  unsigned int fZ = (int)floor(z);
  unsigned int cZ = (int)ceil(z);

  vec3 pointA = vec3(fX, coord2height(tex, fX, fZ), fZ);
  vec3 pointB = vec3(cX, coord2height(tex, cX, fZ), fZ);
  vec3 pointC = vec3(fX, coord2height(tex, fX, cZ), cZ);
  vec3 pointD = vec3(cX, coord2height(tex, cX, cZ), cZ);

  // log
  // printf("x: %f, z: %f\n", x, z);
  // log y values
  // printf("A: %f, B: %f, C: %f, D: %f\n", pointA.y, pointB.y, pointC.y,
  //       pointD.y);

  double s = x - fX;
  double t = z - fZ;

  // printf("s: %f, t: %f\n", s, t);

  // check if we are in the upper or lower triangle
  GLfloat output;
  if (s + t <= 1) {
    // upper triangle
    output = ((1 - s) + (1 - t)) * pointA.y + s * pointB.y + t * pointC.y;
    // printf("upper triangle: %f\n", 0.5 * output);
  } else {
    // lower triangle
    output = (s + t) * pointD.y + (1 - s) * pointC.y + (1 - t) * pointB.y;
    // printf("lower triangle: %f\n", 0.5 * output);
  }
  return 0.5 * output;
}

Model *GenerateTerrain(TextureData *tex) {
  int vertexCount = tex->width * tex->height;
  int triangleCount = (tex->width - 1) * (tex->height - 1) * 2;
  unsigned int x, z;

  vec3 *vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
  GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount * 3);

  printf("bpp %d\n", tex->bpp);
  for (x = 0; x < tex->width; x++)
    for (z = 0; z < tex->height; z++) {
      // Vertex array. You need to scale this properly
      vertexArray[(x + z * tex->width)].x = x / 1.0;
      vertexArray[(x + z * tex->width)].y = coord2height(tex, x, z);
      vertexArray[(x + z * tex->width)].z = z / 1.0;

      // read neightbor heights using an arbitrary small offset
      vec3 off = vec3(1.0, 0.0, 1.0);
      if (x == 0 || x == tex->width - 1) off.x = 0.0;
      if (z == 0 || z == tex->height - 1) off.z = 0.0;
      float hL = coord2height(tex, x - off.x, z - off.y);
      float hR = coord2height(tex, x + off.x, z + off.y);
      float hD = coord2height(tex, x - off.y, z - off.z);
      float hU = coord2height(tex, x + off.y, z + off.z);

      // deduce terrain normal
      vec3 N = normalize(vec3(hL - hR, 2.0, hD - hU));
      normalArray[(x + z * tex->width)].x = N.x;
      normalArray[(x + z * tex->width)].y = N.y;
      normalArray[(x + z * tex->width)].z = N.z;

      // Texture coordinates. You may want to scale them.
      texCoordArray[(x + z * tex->width)].x =
          (float)x / tex->width;  // (float)x / tex->width;
      texCoordArray[(x + z * tex->width)].y =
          (float)z / tex->height;  // (float)z / tex->height;
    }
  for (x = 0; x < tex->width - 1; x++)
    for (z = 0; z < tex->height - 1; z++) {
      // Triangle 1
      indexArray[(x + z * (tex->width - 1)) * 6 + 0] = x + z * tex->width;
      indexArray[(x + z * (tex->width - 1)) * 6 + 1] = x + (z + 1) * tex->width;
      indexArray[(x + z * (tex->width - 1)) * 6 + 2] = x + 1 + z * tex->width;
      // Triangle 2
      indexArray[(x + z * (tex->width - 1)) * 6 + 3] = x + 1 + z * tex->width;
      indexArray[(x + z * (tex->width - 1)) * 6 + 4] = x + (z + 1) * tex->width;
      indexArray[(x + z * (tex->width - 1)) * 6 + 5] =
          x + 1 + (z + 1) * tex->width;
    }

  // End of terrain generation

  // Create Model and upload to GPU:

  Model *model = LoadDataToModel(vertexArray, normalArray, texCoordArray, NULL,
                                 indexArray, vertexCount, triangleCount * 3);

  return model;
}

vec3 cameraDirection(float theta, float phi) {
  return (cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi));
}

void init(void) {
  // GL inits
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);

  float deltaX = 0 - 40;  // From camera to origin
  float deltaZ = 0 + 60;  // From camera to origin (z-component is the same)
  float deltaY = 0 - 10;  // Vertical difference

  // Theta calculation
  // In this scenario, theta would be directly along the -x axis, meaning we
  // look from positive x towards the origin. Since standard theta is measured
  // from the z-axis, facing directly along the negative x-axis is -90 degrees
  // or 270 degrees in standard orientation.
  theta = atan2(deltaZ, deltaX);  // atan2 handles full circle calculations

  // Phi calculation
  // Calculating the angle with respect to the y-axis (vertical elevation)
  float distance =
      sqrt(deltaX * deltaX + deltaZ * deltaZ);  // Distance in the xz-plane
  phi = atan2(deltaY, distance);  // atan2 handles the sign and division
  vec3 angles = cameraDirection(theta, phi);
  cameraLookAt = cameraPos + angles;

  cameraMatrix = lookAtv(cameraPos, cameraLookAt, cameraUp);

  // Load and compile shader
  terrainProgram = loadShaders("terrain.vert", "terrainSplat.frag");
  glUseProgram(terrainProgram);
  printError("init shader");

  loadModels();

  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1,
                     GL_TRUE, projectionMatrix.m);

  // Assign texture units
  glUniform1i(glGetUniformLocation(terrainProgram, "map"),
              0);  // Texture unit 0
  glUniform1i(glGetUniformLocation(terrainProgram, "grass"),
              1);  // Texture unit 1
  glUniform1i(glGetUniformLocation(terrainProgram, "dirt"),
              2);  // Texture unit 2
  glUniform1i(glGetUniformLocation(terrainProgram, "conc"),
              3);  // Texture unit 3

  // Load terrain data
  LoadTGATextureData("heightmaps/fft-terrain1024c.tga", &ttex);
  terrainModel = GenerateTerrain(&ttex);
  printError("init terrain");

  printf(
      "Note: The call to DrawModel will report warnings about inNormal not "
      "existing. This is because inNormal is not used in the shader yet so it "
      "is optimized away.\n");
}

void drawGround() {
  mat4 total, modelView;
  glUseProgram(terrainProgram);
  // Build matrix
  modelView = IdentityMatrix();
  total = cameraMatrix * modelView;
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1,
                     GL_TRUE, total.m);
  // Textures
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("splatmaps/map2.tga", &map);
  glActiveTexture(GL_TEXTURE1);
  LoadTGATextureSimple("textures/grass_smol.tga", &grass);
  glActiveTexture(GL_TEXTURE2);
  LoadTGATextureSimple("textures/dirt.tga", &dirt);
  glActiveTexture(GL_TEXTURE3);
  LoadTGATextureSimple("textures/conc.tga", &conc);
  // Draw
  DrawModel(terrainModel, terrainProgram, "inPosition", "inNormal",
            "inTexCoord");
}

void drawGroundSphere() {
  glUseProgram(terrainProgram);
  float timeFactor = isSlow ? 100 : 1;
  groundBallPos.x += (t - groundBallPos.x) / timeFactor;
  groundBallPos.z += (t - groundBallPos.z) / timeFactor;
  groundBallPos.y = coord2height(&ttex, groundBallPos.x, groundBallPos.z);
  mat4 trans = T(groundBallPos.x, groundBallPos.y, groundBallPos.z);
  mat4 total = cameraMatrix * trans;
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1,
                     GL_TRUE, total.m);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, conc);  // Bind Our Texture tex2
  DrawModel(groundSphereModel, terrainProgram, "inPosition", "inNormal",
            "inTexCoord");
}

void display(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawGround();
  drawGroundSphere();

  printError("display");

  glutSwapBuffers();
}

void mouse(int x, int y) {
  // This function is included in case you want some hints about using passive
  // mouse movement. Uncomment to see mouse coordinates: printf("%d %d\n", x,
  // y);
}

void checkInput() {
  float cameraSpeed = 0.6f;     // Speed of camera movement
  float rotationSpeed = 0.02f;  // Speed of camera rotation
  bool piThetaUpdated = false;

  // Update camera position based on WASD keys
  vec3 forward =
      normalize(cameraLookAt - cameraPos);  // Direction camera is facing
  forward.y = 0;                            // Remove y-component
  forward = normalize(forward);  // Normalize since we altered the length
  vec3 rightDir = normalize(cross(
      forward, cameraUp));  // Right direction relative to camera's forward

  // Forward and backward
  if (glutKeyIsDown('w')) cameraPos += forward * cameraSpeed;
  if (glutKeyIsDown('s')) cameraPos -= forward * cameraSpeed;
  // Right and left (strafe)
  if (glutKeyIsDown('a')) cameraPos -= rightDir * cameraSpeed;
  if (glutKeyIsDown('d')) cameraPos += rightDir * cameraSpeed;

  // Update camera orientation based on IJKL keys
  // Rotate left and right (around the up axis)
  if (glutKeyIsDown('j')) theta -= rotationSpeed;
  if (glutKeyIsDown('l')) theta += rotationSpeed;

  // Rotate up and down (around the right axis)
  if (glutKeyIsDown('i')) phi += rotationSpeed;
  if (glutKeyIsDown('k')) phi -= rotationSpeed;

  if (glutKeyIsDown(GLUT_KEY_SPACE)) {
    isSlow = !isSlow;
  }

  // Clamp the rotation angle to be within reasonable values
  phi = fmax(-M_PI_2 + 0.01, fmin(M_PI_2 - 0.01, phi));
  // make sure theta stays within 0 to 2pi and is positive
  theta = fmod(theta, 2 * M_PI);  // fmax(0.f, fmod(theta, 2 * M_PI));

  // Calculate new camera target based on orientation
  cameraLookAt.x = cameraPos.x + cos(phi) * cos(theta);
  cameraLookAt.y = cameraPos.y + sin(phi);
  cameraLookAt.z = cameraPos.z + cos(phi) * sin(theta);

  // Finally, update the camera matrix with the new position and target
  cameraMatrix = lookAtv(cameraPos, cameraLookAt, cameraUp);
}

void onTimer(int value) {
  // pass time
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 200;
  // check input
  checkInput();

  glutPostRedisplay();
  glutTimerFunc(20, &onTimer, value);  // 50 FPS
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(600, 600);
  glutCreateWindow("TSBK07 Lab 4");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &onTimer, 0);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
