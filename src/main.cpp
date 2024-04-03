// Lab 4, terrain generation

// uses framework Cocoa
// uses framework OpenGL
#define MAIN

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "MicroGlut.h"
#include "boxes.h"
#include "ground.h"
#include "vector"

// time
GLfloat t;
// matrices
mat4 cameraMatrix, projectionMatrix;
// vertex array object
Model *groundSphereModel, *skyboxModel, *groundModel;
// Reference to shader program
GLuint terrainProgram, noShadeProgram;
// Reference to textures
GLuint backroomsWallTex, backroomsFloorTex, skyboxTex, grassTex;

// camera
vec3 cameraPos(60.f, 10.f, 0.f);
vec3 cameraLookAt(0.f, 10.f, 0.f);
vec3 cameraUp(0.f, 1.f, 0.f);
vec3 groundBallPos = vec3(0, 0, 0);

float theta = -2 * M_PI_2;
float phi = 0;
bool isSlow = false;

struct ObjectManager {
  std::vector<Model *> objects;
  std::vector<mat4> translations;
  std::vector<mat4> rotations;
  std::vector<GLuint> texUnits;
  std::vector<GLuint *> programs;
};

struct LightManager {
  std::vector<vec3> lightSourcesColors;
  std::vector<GLint> isDirectional;
  std::vector<vec3> lightSourcesDirectionsPositions;
  int getCount() { return lightSourcesColors.size(); }
};

ObjectManager objectManager;
LightManager lightManager;

void generateBoxes() {
  // wall 1
  objectManager.objects.push_back(getBoxModel(100, 50, 2));
  objectManager.translations.push_back(T(-50, 0, -50));
  objectManager.rotations.push_back(Ry(0));
  objectManager.programs.push_back(&terrainProgram);
  objectManager.texUnits.push_back(1);
  // wall 2
  objectManager.objects.push_back(getBoxModel(100, 50, 2));
  objectManager.translations.push_back(T(-50, 0, 50));
  objectManager.rotations.push_back(Ry(M_PI / 2));
  objectManager.programs.push_back(&terrainProgram);
  objectManager.texUnits.push_back(1);
  // wall 3
  objectManager.objects.push_back(getBoxModel(100, 50, 2));
  objectManager.translations.push_back(T(-50, 0, -50));
  objectManager.rotations.push_back(Ry(M_PI / 2));
  objectManager.programs.push_back(&terrainProgram);
  objectManager.texUnits.push_back(1);
  // ceiling
  objectManager.objects.push_back(getBoxModel(100, 2, 100));
  objectManager.translations.push_back(T(-50, 50, -50));
  objectManager.rotations.push_back(Ry(0));
  objectManager.programs.push_back(&terrainProgram);
  objectManager.texUnits.push_back(1);
}

/// @brief Load models from files: ground sphere and ground plane
void loadModels() {
  groundSphereModel = LoadModelPlus("objects/groundsphere.obj");
  skyboxModel = LoadModelPlus("objects/skybox.obj");
  groundModel = getGroundModel(50.0);
  generateBoxes();
}

/// @brief Load textures from file
void loadTextures() {
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("textures/floor.tga", &backroomsFloorTex);
  glActiveTexture(GL_TEXTURE1);
  LoadTGATextureSimple("textures/wall.tga", &backroomsWallTex);
  glActiveTexture(GL_TEXTURE2);
  LoadTGATextureSimple("textures/skybox.tga", &skyboxTex);
  glActiveTexture(GL_TEXTURE3);
  LoadTGATextureSimple("textures/grass.tga", &grassTex);
}

/// @brief  Calculate the direction of the camera
/// @param theta
/// @param phi
/// @return
vec3 cameraDirection(float theta, float phi) { return (cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi)); }

void initShaders() {
  terrainProgram = loadShaders("terrain.vert", "terrain.frag");
  noShadeProgram = loadShaders("noShade.vert", "noShade.frag");
  printError("init shader");
}

/// @brief Upload the projection matrix to the shader programs
void uploadProjectionMatrix() {
  glUseProgram(terrainProgram);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUseProgram(noShadeProgram);
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

int getLightCount(const LightManager &lightManager) { return lightManager.lightSourcesColors.size(); }

void placeLight(vec3 lightPos, vec3 lightColor, bool isDirectional = false) {
  lightManager.lightSourcesColors.push_back(lightColor);
  lightManager.lightSourcesDirectionsPositions.push_back(lightPos);
  GLint isDir = isDirectional ? 1 : 0;
  lightManager.isDirectional.push_back(isDir);
}

void uploadLights() {
  glUseProgram(terrainProgram);
  int lightCount = getLightCount(lightManager);
  glUniform1i(glGetUniformLocation(terrainProgram, "lightCount"), lightCount);
  glUniform3fv(glGetUniformLocation(terrainProgram, "lightSourcesColors"), lightCount,
               &lightManager.lightSourcesColors[0].x);
  glUniform3fv(glGetUniformLocation(terrainProgram, "lightSourcesDirPos"), lightCount,
               &lightManager.lightSourcesDirectionsPositions[0].x);
  glUniform1iv(glGetUniformLocation(terrainProgram, "isDirectional"), lightCount, &lightManager.isDirectional[0]);
}

void init(void) {
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);
  vec3 angles = cameraDirection(theta, phi);
  cameraLookAt = cameraPos + angles;
  cameraMatrix = lookAtv(cameraPos, cameraLookAt, cameraUp);

  initShaders();
  uploadProjectionMatrix();

  placeLight(vec3(0, 40, 25), vec3(1, 1, 1));
  uploadLights();
  printError("init light");

  loadTextures();
  loadModels();
  printError("init");
}
void drawSkybox() {
  glUseProgram(noShadeProgram);
  // disable depth test and culling
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // camera matrix without translation
  mat4 skyBoxTransform = cameraMatrix;

  // remove translation
  skyBoxTransform.m[3] = 0;
  skyBoxTransform.m[7] = 0;
  skyBoxTransform.m[11] = 0;

  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "cameraMatrix"), 1, GL_TRUE, skyBoxTransform.m);

  mat4 trans = T(0.0f, -0.3f, 0.0f);
  mat4 rot = Ry(0);
  mat4 total = rot * trans;
  mat4 scale = S(20.0f, 20.0f, 20.0f);
  total = total * scale;
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
  // set texture as skybox texture (texUnit = 2)
  glUniform1i(glGetUniformLocation(noShadeProgram, "texUnit"), 2);
  glUniformMatrix4fv(glGetUniformLocation(noShadeProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  DrawModel(skyboxModel, noShadeProgram, "inPosition", NULL, "inTexCoord");

  // enable depth test and culling
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void drawGround() {
  glUseProgram(terrainProgram);
  mat4 trans = T(0.f, 0.f, 0.f);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, cameraMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, trans.m);
  // set texture as floor texture (texUnit = 0)
  glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), 0);
  DrawModel(groundModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
}

void drawGroundSphere() {
  glUseProgram(terrainProgram);
  groundBallPos.x += (t - groundBallPos.x);
  groundBallPos.z += (t - groundBallPos.z);
  groundBallPos.y = 0;
  mat4 trans = T(groundBallPos.x, groundBallPos.y, groundBallPos.z);
  mat4 total = cameraMatrix * trans;
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, trans.m);

  // set texture as wall texture
  glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), 3);
  DrawModel(groundSphereModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
}

void drawObjects() {
  for (int i = 0; i < objectManager.objects.size(); i++) {
    Model *model = objectManager.objects[i];
    mat4 trans = objectManager.translations[i];
    mat4 rot = objectManager.rotations[i];
    GLuint program = *objectManager.programs[i];
    GLuint texUnit = objectManager.texUnits[i];
    glUseProgram(program);
    mat4 m2w = trans * rot;
    mat4 total = cameraMatrix * m2w;
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "model2World"), 1, GL_TRUE, m2w.m);
    // set texture as wall texture (texUnit = 1)
    glUniform1i(glGetUniformLocation(terrainProgram, "texUnit"), texUnit);
    DrawModel(model, terrainProgram, "inPosition", "inNormal", "inTexCoord");
  }
}

void display(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawSkybox();
  drawGroundSphere();
  drawGround();
  drawObjects();

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
  vec3 forward = normalize(cameraLookAt - cameraPos);   // Direction camera is facing
  forward.y = 0;                                        // Remove y-component
  forward = normalize(forward);                         // Normalize since we altered the length
  vec3 rightDir = normalize(cross(forward, cameraUp));  // Right direction relative to camera's forward

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

  if (glutKeyIsDown(GLUT_KEY_UP)) {
    cameraPos.y += cameraSpeed;
  } else if (glutKeyIsDown(GLUT_KEY_DOWN)) {
    cameraPos.y -= cameraSpeed;
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
  glutCreateWindow("TSBK07 - Project");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &onTimer, 0);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
