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

// camera
vec3 cameraPos(60.f, 10.f, 0.f);
vec3 cameraLookAt(0.f, 10.f, 0.f);
vec3 cameraUp(0.f, 1.f, 0.f);
float theta = -M_PI_2;
float phi = 0;

mat4 projectionMatrix;

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
      vertexArray[(x + z * tex->width)].y =
          tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 100.0;
      vertexArray[(x + z * tex->width)].z = z / 1.0;
      // Normal vectors. You need to calculate these.
      normalArray[(x + z * tex->width)].x = 0.0;
      normalArray[(x + z * tex->width)].y = 1.0;
      normalArray[(x + z * tex->width)].z = 0.0;
      // Texture coordinates. You may want to scale them.
      texCoordArray[(x + z * tex->width)].x = x;  // (float)x / tex->width;
      texCoordArray[(x + z * tex->width)].y = z;  // (float)z / tex->height;
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

// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex;  // terrain

vec3 cameraDirection(float theta, float phi) {
  return (cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi));
}

void init(void) {
  // GL inits
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

  float deltaX = 0 - 60;  // From camera to origin
  float deltaZ = 0 - 0;   // From camera to origin (z-component is the same)
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
  program = loadShaders("terrain.vert", "terrain.frag");
  glUseProgram(program);
  printError("init shader");

  glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE,
                     projectionMatrix.m);
  glUniform1i(glGetUniformLocation(program, "tex"), 0);  // Texture unit 0
  LoadTGATextureSimple("maskros512.tga", &tex1);

  // Load terrain data

  LoadTGATextureData("fft-terrain.tga", &ttex);
  tm = GenerateTerrain(&ttex);
  printError("init terrain");

  printf(
      "Note: The call to DrawModel will report warnings about inNormal not "
      "existing. This is because inNormal is not used in the shader yet so it "
      "is optimized away.\n");
}

void display(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 total, modelView;

  printError("pre display");

  glUseProgram(program);

  // Build matrix
  modelView = IdentityMatrix();
  total = cameraMatrix * modelView;
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE,
                     total.m);

  glBindTexture(GL_TEXTURE_2D, tex1);  // Bind Our Texture tex1
  DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

  printError("display 2");

  glutSwapBuffers();
}

void mouse(int x, int y) {
  // This function is included in case you want some hints about using passive
  // mouse movement. Uncomment to see mouse coordinates: printf("%d %d\n", x,
  // y);
}

void checkInput() {
  float cameraSpeed = 0.3f;     // Speed of camera movement
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
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) * M_PI / (5 * 1000);
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
