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
GLuint map, backroomsWallTex, backroomsFloorTex;
TextureData ttex;  // terrain

// camera
vec3 cameraPos(60.f, 40.f, 0.f);
vec3 cameraLookAt(0.f, 10.f, 0.f);
vec3 cameraUp(0.f, 1.f, 0.f);
vec3 groundBallPos = vec3(0, 0, 0);
float theta = -M_PI_2;
float phi = 0;
bool isSlow = false;

Model *groundModel, *skyBox, *teaPot;

mat4 projectionMatrix;
#define kGroundSize 100.0f

/// @brief Load models from files: ground sphere and ground plane
void loadModels() {
  groundSphereModel = LoadModelPlus("objects/groundsphere.obj");
  vec3 vertices[] = {vec3(-kGroundSize, 0.0f, -kGroundSize),
                     vec3(-kGroundSize, 0.0f, kGroundSize),
                     vec3(kGroundSize, -0.0f, -kGroundSize),
                     vec3(kGroundSize, -0.0f, kGroundSize)};

  vec3 vertex_normals[] = {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                           vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)};

  vec2 tex_coords[] = {vec2(0.0f, 0.0f), vec2(0.0f, 20.0f), vec2(20.0f, 0.0f),
                       vec2(20.0f, 20.0f)};
  GLuint indices[] = {0, 1, 2, 1, 3, 2};
  vec3 colors[] = {(100.f, 0.f, 0.f)};
  int numVert = 4, numInd = 6;
  groundModel = LoadDataToModel(vertices, vertex_normals, tex_coords, colors,
                                indices, numVert, numInd);
}

/// @brief Load textures from file
void loadTextures() {
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("textures/floor.tga", &backroomsFloorTex);
  glActiveTexture(GL_TEXTURE1);
  LoadTGATextureSimple("textures/wall.tga", &backroomsWallTex);
}

/// @brief  Calculate the direction of the camera
/// @param theta
/// @param phi
/// @return
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

  theta = 0;
  phi = 0;
  vec3 angles = cameraDirection(theta, phi);
  cameraLookAt = cameraPos + angles;

  cameraMatrix = lookAtv(cameraPos, cameraLookAt, cameraUp);

  // Load and compile shader
  terrainProgram = loadShaders("terrain.vert", "terrain.frag");
  glUseProgram(terrainProgram);
  printError("init shader");

  loadTextures();
  loadModels();

  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1,
                     GL_TRUE, projectionMatrix.m);

  // Load terrain data
  LoadTGATextureData("heightmaps/fft-terrain1024c.tga", &ttex);
  // terrainModel = GenerateTerrain(&ttex);
  printError("init terrain");
}

void drawGround() {
  glUseProgram(terrainProgram);
  mat4 trans = T(0.f, 0.f, 0.f);
  mat4 total = cameraMatrix * IdentityMatrix();
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1,
                     GL_TRUE, total.m);
  // set texture as floor texture
  glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0);
  DrawModel(groundModel, terrainProgram, "inPosition", "inNormal", NULL);
}

void drawGroundSphere() {
  glUseProgram(terrainProgram);
  float timeFactor = isSlow ? 100 : 1;
  groundBallPos.x += (t - groundBallPos.x) / timeFactor;
  groundBallPos.z += (t - groundBallPos.z) / timeFactor;
  groundBallPos.y = 0;
  mat4 trans = T(groundBallPos.x, groundBallPos.y, groundBallPos.z);
  mat4 total = cameraMatrix * trans;
  glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1,
                     GL_TRUE, total.m);
  // set texture as wall texture
  glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 1);
  DrawModel(groundSphereModel, terrainProgram, "inPosition", "inNormal",
            "inTexCoord");
}

void display(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // drawTerrain();
  drawGroundSphere();
  drawGround();

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
  glutCreateWindow("TSBK07 - Project");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &onTimer, 0);
  glutPassiveMotionFunc(mouse);
  glutMainLoop();
  exit(0);
}
