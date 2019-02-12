/**
 * @author	Andrew Robert Owens
 * @date January, 2019
 * @brief	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Contact:	arowens [at] ucalgary.ca
 *
 * Copyright (c) 2019 - Please give credit to the author.
 *
 * @file	main.cpp
 *
 * @brief
 * This is a (very) basic program to
 * 1) load shaders from external files, and make a shader program
 * 2) load a mesh (.obj)
 * 3) make Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for the mesh
 * 4) load a curve that was created
 * 5) make VAO and VBO for the curve
 * 6) animate the mesh around the curve
 *
 * take a look at the following sites for further readings:
 * learnopengl.com -> AWESOME
 * opengl-tutorial.org -> The first triangle (New OpenGL, great start)
 * antongerdelan.net -> shaders pipeline explained
 * ogldev.atspace.co.uk -> good resource
 */

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>
#include <algorithm>

#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "curve.h"
#include "curvefileio.h"
#include "mat4f.h"
#include "openglmatrix.h"
#include "program.h"
#include "vec3f.h"

//http://irrlicht.sourceforge.net/forum/viewtopic.php?p=145834

//==================== GLOBAL VARIABLES ====================//
/*	Put here for simplicity. Feel free to restructure into
 *	appropriate classes or abstractions (Do do as I say, not as I do do..)
 */

// Drawing Programs manager
std::vector<opengl::Program> g_program;

namespace openGL {
// Data needed rendering for mesh
struct RenderableMesh {
  GLuint vaoID = 0;
  GLuint vertexBufferID = 0;
  GLuint indexBufferID = 0;
  GLuint verticesCount = 0;
  GLuint indicesCount = 0;
  math::Mat4f modelMatrix = math::identity();
};

// Data needed rendering for curve
struct RenderableLine {
  GLuint vaoID = 0;
  GLuint vertexBufferID = 0;
  GLuint verticesCount = 0;
  math::Mat4f modelMatrix = math::identity();
};
} // namespace openGL

openGL::RenderableMesh g_meshData;
openGL::RenderableLine g_curveData;

// Curve geometry for simulation
std::string g_curveFilePath = "./curves/RollerCosterTrack.obj";
//std::string g_curveFilePath = "./curves/RollerCosterTrackv2DemonHorn.obj";
math::geometry::Curve g_curve;
math::geometry::Curve g_curveLT;
math::geometry::Curve g_curveRT;
math::geometry::Curve g_curvePlanks;
math::geometry::Curve g_pillar;
int32_t g_numberOfSubdivisions = 0;

// Only one camera, so only one veiw and perspective matrix are needed.
math::Mat4f g_V;
math::Mat4f g_P;

// Only one thing is rendered at a time, so only need one MVP
// When drawing different objects, update M and MVP = M * V * P
math::Mat4f g_MVP;

// Camera and veiwing Stuff
openGL::scene::Camera g_camera;
openGL::scene::CameraUpdate g_cameraUpdate;

float g_rotationSpeed = 1.f;
float g_panningSpeed = 0.25f;
float g_cursorSpeed = 0.05f;
bool g_cursorLocked;
float g_cursorX, g_cursorY;

bool g_play = false;

int WIN_WIDTH = 800, WIN_HEIGHT = 600;
int FB_WIDTH = 800, FB_HEIGHT = 600;
float WIN_FOV = 50.f;
float WIN_NEAR = 0.01f;
float WIN_FAR = 100.f;

float scale = 0.001;
//unsigned int now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
unsigned int lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
unsigned int currentTime = lastTime;
float fps = 60;

float arcLengthS = 0.01;
bool resetVel = true;


float grav = 9.81*0.00005;
math::Vec3f gravMat = {0, -grav, 0};
float mass = 1;

float vel = 0.01;



float carH = 0.25;
float carW = 0.5;
float carL = 1;


math::Vec3f binormal = math::Vec3f{1,0,0};
math::Vec3f normal = math::Vec3f{0,1,0};
math::Vec3f tangent = math::Vec3f{0,0,1};

math::Vec3f binormalN = math::Vec3f{1,0,0};
math::Vec3f normalN = math::Vec3f{0,1,0};
math::Vec3f tangentN = math::Vec3f{0,0,1};

math::Vec3f lastPosition;
math::Vec3f currentPosition;
math::Vec3f lastTangent;

float constantVel = 1/fps;

int cameraMode = 0;         //1 is free,    2 is cinamatic,    3 is passenger
int cartCheckpoint = 1;

//==================== FUNCTION DECLARATIONS ====================//
void displayFunc();
void resizeFunc();

bool init();
bool generateIDs();
void deleteIDs();
void setupVAO();

bool loadMeshGeometryToGPU();
bool loadCurveGeometryToGPU();
bool loadCurveGeometryToGPU(math::geometry::Curve curve);

void updateCheckpoint(int curveVertexID);
void updateCamera();
void updateVel(uint32_t curveVertexID);
void updateBasisVectors(math::Vec3f lastPoint, math::Vec3f currentPoint, math::Vec3f nextPoint);

math::geometry::Curve arcLengthParam(math::geometry::Curve curve, float deltaS);
float length(math::Vec3f);
math::Mat4f TRMatrix(math::Vec3f const &binormal, math::Vec3f const &normal, math::Vec3f const &tangent, math::Vec3f const &pos);



void reloadProjectionMatrix();
void reloadViewMatrix();

void windowSetSizeFunc();
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowSetSizeFunc(GLFWwindow *window, int width, int height);
void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height);
void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void animate(int t);
void simulationStep(int t);
void moveCamera();
void resetCamera();

std::string GL_ERROR();
int main(int, char **);



using namespace irrklang;
ISoundEngine *engine;
//==================== FUNCTION DEFINITIONS ====================//

void displayFunc() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader (must be called BEFORE setting uniforms!!!)
  auto &program = g_program[0];
  program.use();

  // ===== DRAW MESH ====== //
  g_MVP = g_P * g_V * g_meshData.modelMatrix;
  program.setUniformMat4f("MVP", g_MVP,
                          true); // true, transpose for stupid OpenGL

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(g_meshData.vaoID);
  // Draw mesh, start at vertex 0, draw a # of them
  glDrawElements(GL_TRIANGLE_STRIP, g_meshData.indicesCount, GL_UNSIGNED_INT,
                 (void *)0);

  // ==== DRAW CURVE ===== //
  g_MVP = g_P * g_V * g_curveData.modelMatrix;
  program.setUniformMat4f("MVP", g_MVP,
                          true); // true, transpose for stupid OpenGL

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(g_curveData.vaoID);
  // Draw lines
  loadCurveGeometryToGPU(g_curveLT);
  glDrawArrays(GL_LINE_LOOP, 0, g_curveData.verticesCount);

  loadCurveGeometryToGPU(g_curveRT);
  glDrawArrays(GL_LINE_LOOP, 0, g_curveData.verticesCount);

  loadCurveGeometryToGPU(g_curvePlanks);
  glDrawArrays(GL_TRIANGLES, 0, g_curveData.verticesCount);

  loadCurveGeometryToGPU(g_pillar);
  glDrawArrays(GL_TRIANGLES, 0, g_curveData.verticesCount);

}

void animate(int vertexID) {
  using namespace openGL;
  math::Vec3f pos = g_curve[vertexID];

  g_meshData.modelMatrix = TRMatrix( tangentN,  normalN, binormalN,pos) * UniformScaleMatrix(0.1f);
}

//Binormal, normal, tangent, position
math::Mat4f TRMatrix(math::Vec3f const &binormal, math::Vec3f const &normal, math::Vec3f const &tangent, math::Vec3f const &pos) {
    using namespace math;
  Mat4f trans = {
      binormal.m_x, normal.m_x, tangent.m_x, pos.m_x, // 1 0 0 x
      binormal.m_y, normal.m_y, tangent.m_y, pos.m_y, // 0 1 0 y
      binormal.m_z, normal.m_z, tangent.m_z, pos.m_z, // 0 0 1 z
      0.f, 0.f, 0.f, 1.f      // 0 0 0 1
  };

  return trans;
}


void oncePerFrame() {
  static uint32_t curveVertexID = 0;

  lastPosition = g_curve[curveVertexID];

  //Move cart along track
  curveVertexID += vel/arcLengthS;
  currentPosition = g_curve[curveVertexID];
  std::cout<<"CurveID: " << curveVertexID <<"/"<<g_curve.pointCount() << std::endl;

  updateVel(curveVertexID);
  int nextPoint = ((int)(curveVertexID+vel/arcLengthS))%g_curve.pointCount();
  updateBasisVectors(lastPosition, g_curve[curveVertexID], g_curve[nextPoint]);

  //Loop back to start of track
  if (curveVertexID >= g_curve.pointCount())
    curveVertexID = 0;

  updateCheckpoint(curveVertexID);
  updateCamera();

  animate(curveVertexID);
}


void updateCheckpoint(int curveVertexID){
    if(curveVertexID <= 790){
        cartCheckpoint = 1;
    }
    else if(curveVertexID <= 3600){
        if(cartCheckpoint == 1)
            engine->play2D("external/Scream4.wav", false);
        cartCheckpoint = 2;
    }
    else{
        cartCheckpoint = 3;
    }
}

void updateCamera(){
    if(cameraMode == 1)
    {
        //Allow free-cam
    }

    if(cameraMode == 2)
    {
        g_camera.m_pos = math::Vec3f{0.0, 0.0f, 0.0f};
        g_camera.m_forward = math::normalized(currentPosition - g_camera.m_pos);
        g_camera.m_up = math::Vec3f{0.0, 1.0f, 0.0f};
    }

    else if(cameraMode == 3)
    {
        g_camera.m_pos = currentPosition + normalN*0.05;
        g_camera.m_forward = tangentN;
        g_camera.m_up = normalN;
      }

    reloadViewMatrix();
}


void updateVel(uint32_t curveVertexID){
    //Initial climb
    if(cartCheckpoint == 1){
        if (vel < constantVel)
          vel = vel*1.005;
        resetVel = true;
    }

    //Physics simulation
    else if (cartCheckpoint == 2){
        if(resetVel){
            vel = constantVel;
            resetVel = false;
        }
        if(tangent[1] <=0 ){
            vel += (sqrt(2*grav*(-tangent[1]))/fps);}
        else
            vel -= (sqrt(2*grav*tangent[1])/fps);
    }

    //Ending slowdown
    else{
        if (vel > constantVel)
            vel = vel*0.95;
    }
    std::cout<<"Current speed: " << vel << std::endl;
}


void updateBasisVectors(math::Vec3f lastPoint, math::Vec3f currentPoint, math::Vec3f nextPoint){
    math::Vec3f lastTangent = currentPoint - lastPoint;
    tangent = nextPoint - currentPoint;
    tangentN = math::normalized(tangent);

    lastTangent = math::normalized(lastTangent)*vel;
    tangent = math::normalized(tangent)*vel;

    math::Vec3f perpAccel = tangent - lastTangent;

    normal = perpAccel - gravMat;
    normalN = math::normalized(normal);

    binormalN = math::normalized(cross(tangentN, normalN));
    normalN = math::normalized(cross(binormalN, tangentN));
}



bool loadMeshGeometryToGPU() {
  std::vector<math::Vec3f> verts;

  //Left Side
  verts.push_back({-carL, -carH, carW});
  verts.push_back({-carL, carH, carW});
  verts.push_back({carL, -carH, carW});
  verts.push_back({carL, carH, carW});

  //Back
  verts.push_back({carL, -carH, -carW});
  verts.push_back({carL, carH, -carW});

  //Top
  verts.push_back({carL, carH, carW});
  verts.push_back({-carL, carH, -carW});
  verts.push_back({-carL, carH, carW});

  //Front
  verts.push_back({-carL, -carH, carW});
  verts.push_back({-carL, carH, -carW});
  verts.push_back({-carL, -carH, -carW});

  //Bottom
  verts.push_back({-carL, -carH, carW});
  verts.push_back({carL, -carH, carW});
  verts.push_back({carL, -carH, -carW});

  //Right side
  verts.push_back({-carL, -carH, -carW});
  verts.push_back({carL, carH, -carW});
  verts.push_back({-carL, carH, -carW});

  g_meshData.verticesCount = verts.size();

  glBindBuffer(GL_ARRAY_BUFFER, g_meshData.vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(math::Vec3f) *
                   g_meshData.verticesCount, // byte size of Vec3f
               verts.data(),    // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW); // Usage pattern of GPU buffer

  std::vector<GLuint> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

  g_meshData.indicesCount = indices.size();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_meshData.indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLuint) * g_meshData.indicesCount, indices.data(),
               GL_STATIC_DRAW);

  return true;
}

bool loadCurveGeometryToGPU(int numberOfSubdivisions) {
  using namespace math::geometry;
  if (g_curveFilePath.empty()) {
    std::vector<math::Vec3f> verts;
    verts.push_back({-1.f, -1.f, 0});
    verts.push_back({-1.f, 1.f, 0});
    verts.push_back({1.f, 1.f, 0});
    verts.push_back({1.f, -1.f, 0.f});

    g_curve = Curve{verts};
  } else {
    // auto curve = loadCurveFromFile(g_curveFilePath);
    auto curve = loadCurveFrom_OBJ_File(g_curveFilePath);
    if (curve.pointCount() == 0) {
      std::cerr << "curve is empty\n";
      return false;
    }
    g_curve = Curve(std::move(curve));
  }

  numberOfSubdivisions = 3;
  g_curve = math::geometry::cubicSubdivideCurve(g_curve, numberOfSubdivisions);
  g_curve = arcLengthParam(g_curve, arcLengthS);

  /*
  //Need this stuff
  glBindBuffer(GL_ARRAY_BUFFER, g_curveData.vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(math::Vec3f) *
                   g_curve.pointCount(), // byte size of Vec3f, 4 of them
               g_curve.data(),  // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW); // Usage pattern of GPU buffer
    */
  g_curveData.verticesCount = g_curve.pointCount();

  return true;
}


bool loadCurveGeometryToGPU(math::geometry::Curve curve) {
  using namespace math::geometry;

  //Need this stuff
  glBindBuffer(GL_ARRAY_BUFFER, g_curveData.vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(math::Vec3f) *
                   curve.pointCount(), // byte size of Vec3f, 4 of them
               curve.data(),  // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW); // Usage pattern of GPU buffer


  return true;
}




math::geometry::Curve arcLengthParam(math::geometry::Curve curve, float deltaS){
    std::cout<<"Starting with " << curve.pointCount() << " points"<<std::endl;

    int deltaTSubdivCount = 14;
    math::geometry::Points temp;
    curve = math::geometry::cubicSubdivideCurve(curve, deltaTSubdivCount);

    int counter = 0;
    //math::Vec3f
    float distanceTally;
    for (int i=0; i<curve.pointCount(); i++){

        math::Vec3f directionVec;
        directionVec = curve[(i+1)%curve.pointCount()] - curve[i%curve.pointCount()];

        distanceTally += length(directionVec);

        if(distanceTally >= deltaS){
            distanceTally = 0;
            temp.push_back(curve[i]);
            counter++;
        }
    }
    std::cout<<"Cut into " << counter << " equally spaced points"<<std::endl;

    return math::geometry::Curve(temp);
}

float length(math::Vec3f vector){
    return sqrt(math::dot(vector,vector));
}


void setupVAO() {
  glBindVertexArray(g_meshData.vaoID);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_meshData.indexBufferID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, g_meshData.vertexBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glBindVertexArray(g_curveData.vaoID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, g_curveData.vertexBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glBindVertexArray(0); // reset to default
}

void reloadProjectionMatrix() {
  g_P = openGL::PerspectiveProjection(WIN_FOV, // FOV
                                      static_cast<float>(WIN_WIDTH) /
                                          WIN_HEIGHT, // Aspect
                                      WIN_NEAR,       // near plane
                                      WIN_FAR);       // far plane depth
}

void reloadViewMatrix() { g_V = openGL::scene::makeViewMatrix(g_camera); }

bool reloadShadersFromFile() {
  // will delete shaders from GPU as well (RAII)
  g_program.clear();

  using namespace opengl;
  // shader ID from OpenGL
  auto vsSource = loadShaderStringFromFile("./shaders/basic_vs.glsl");
  auto fsSource = loadShaderStringFromFile("./shaders/basic_fs.glsl");
  if (vsSource.empty() || fsSource.empty()) {
    std::cerr << "Failed to load shaders from file\n";
    return false;
  }
  auto program = makeProgram(vsSource, fsSource);
  if (!program.isValid()) {
    std::cerr << "Failed to load program\n";
    return false;
  }
  g_program.push_back(std::move(program));

  return true;
}

bool generateIDs() {

  if (!reloadShadersFromFile())
    return false;

  // VAO and buffer IDs given from OpenGL
  glGenVertexArrays(1, &g_meshData.vaoID);
  glGenBuffers(1, &g_meshData.vertexBufferID);
  glGenBuffers(1, &g_meshData.indexBufferID);

  glGenVertexArrays(1, &g_curveData.vaoID);
  glGenBuffers(1, &g_curveData.vertexBufferID);

  return true;
}

void deleteIDs() {
  g_program.clear(); // calls destructors on shaders, deallocates GPU

  glDeleteVertexArrays(1, &g_meshData.vaoID);
  glDeleteBuffers(1, &g_meshData.vertexBufferID);
  glDeleteBuffers(1, &g_meshData.indexBufferID);

  glDeleteVertexArrays(1, &g_curveData.vaoID);
  glDeleteBuffers(1, &g_curveData.vertexBufferID);
}


bool init() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // glEnable(GL_MULTISAMPLE);
  glPointSize(50);

  // SETUP SHADERS, BUFFERS, VAOs

  if (!generateIDs())
    return false;

  setupVAO();

  if (!loadMeshGeometryToGPU()) {
    return false;
  }
  if (!loadCurveGeometryToGPU(g_numberOfSubdivisions)) {
    return false;
  }

  int max = g_curve.pointCount();
  math::Vec3f lastPoint = g_curve.m_points[max-1];
  math::Vec3f currentPoint;
  math::Vec3f nextPoint;
  for(int i = 0; i < g_curve.pointCount(); i++){
        currentPoint = g_curve.m_points[(i)%max];
        nextPoint = g_curve.m_points[(i+1)%max];

        updateVel(i);
        updateBasisVectors(lastPoint, currentPoint, nextPoint);

        //std::cout << "Binormal of track: " << binormalN[0] << " " << binormalN[1] << " " << binormalN[2] << std::endl;

        math::Vec3f leftTrackPoint = g_curve.m_points[i] - normalN*carH*0.1 + binormalN*carW*0.1;
        math::Vec3f rightTrackPoint = g_curve.m_points[i] - normalN*carH*0.1 - binormalN*carW*0.1;

        math::Vec3f leftFutureTrackPoint = g_curve.m_points[i+2] - normalN*carH*0.1 + binormalN*carW*0.1;
        math::Vec3f rightFutureTrackPoint = g_curve.m_points[i+2] - normalN*carH*0.1 - binormalN*carW*0.1;

        g_curveLT.m_points.push_back(leftTrackPoint);
        g_curveRT.m_points.push_back(rightTrackPoint);

        if(i%8 == 0){
            g_curvePlanks.m_points.push_back(leftTrackPoint);
            g_curvePlanks.m_points.push_back(rightTrackPoint);
            g_curvePlanks.m_points.push_back(leftFutureTrackPoint);

            g_curvePlanks.m_points.push_back(leftFutureTrackPoint);
            g_curvePlanks.m_points.push_back(rightFutureTrackPoint);
            g_curvePlanks.m_points.push_back(rightTrackPoint);
        }

        if(i%32 == 0){
            //Left pillar
            g_pillar.m_points.push_back(leftTrackPoint);
            g_pillar.m_points.push_back(math::Vec3f(leftTrackPoint[0], 0.0f, leftTrackPoint[2]));
            g_pillar.m_points.push_back(math::Vec3f(leftFutureTrackPoint[0], 0.0f, leftFutureTrackPoint[2]));

            g_pillar.m_points.push_back(math::Vec3f(leftFutureTrackPoint[0], 0.0f, leftFutureTrackPoint[2]));
            g_pillar.m_points.push_back(leftFutureTrackPoint);
            g_pillar.m_points.push_back(leftTrackPoint);

            //Right pillar
            g_pillar.m_points.push_back(rightTrackPoint);
            g_pillar.m_points.push_back(math::Vec3f(rightTrackPoint[0], 0.0f, rightTrackPoint[2]));
            g_pillar.m_points.push_back(math::Vec3f(rightFutureTrackPoint[0], 0.0f, rightFutureTrackPoint[2]));

            g_pillar.m_points.push_back(math::Vec3f(rightFutureTrackPoint[0], 0.0f, rightFutureTrackPoint[2]));
            g_pillar.m_points.push_back(rightFutureTrackPoint);
            g_pillar.m_points.push_back(rightTrackPoint);
        }

        lastPoint = currentPoint;
  }

  resetCamera();

  reloadProjectionMatrix();
  reloadViewMatrix();

  return true;
}

int main(int argc, char **argv) {

  if (argc == 2)
    g_curveFilePath = {argv[1]};

  GLFWwindow *window;

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }


  // start the sound engine with default parameters
  engine = createIrrKlangDevice();

  if (!engine)
     return 0; // error starting up the engine


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //  glfwWindowHint(GLFW_SAMPLES, 4);

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CPSC 587/687 Flying Susan",
                            NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwGetWindowSize(window, &WIN_WIDTH, &WIN_HEIGHT);
  glfwGetFramebufferSize(window, &FB_WIDTH, &FB_HEIGHT);

  glfwSetWindowSizeCallback(window, windowSetSizeFunc);
  glfwSetFramebufferSizeCallback(window, windowSetFramebufferSizeFunc);
  glfwSetKeyCallback(window, windowKeyFunc);
  glfwSetCursorPosCallback(window, windowMouseMotionFunc);
  glfwSetMouseButtonCallback(window, windowMouseButtonFunc);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1); // vsync

  std::cout << "GL Version: :" << glGetString(GL_VERSION) << std::endl;
  std::cout << GL_ERROR() << std::endl;

  // Initialize all the geometry, and load it once to the GPU
  if (init()) // our own initialize stuff func
  {

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           !glfwWindowShouldClose(window)) {

      if (g_play) {
          currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          if(currentTime >= lastTime+((1/fps)*1000))
            oncePerFrame();
      }

      displayFunc();
      moveCamera();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  // clean up after loop
  deleteIDs();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}




//==================== CALLBACK FUNCTIONS ====================//

void windowSetSizeFunc(GLFWwindow *window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  reloadProjectionMatrix();
}

void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height) {
  FB_WIDTH = width;
  FB_HEIGHT = height;

  glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
}

void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      g_cursorLocked = GL_TRUE;
    } else {
      g_cursorLocked = GL_FALSE;
    }
  }
}

void windowMouseMotionFunc(GLFWwindow *window, double x, double y) {
  if (g_cursorLocked) {
    float deltaX = (x - g_cursorX) * g_cursorSpeed;
    float deltaY = (y - g_cursorY) * g_cursorSpeed;
    g_camera.rotateRightAroundFocus(deltaX);
    g_camera.rotateDownAroundFocus(deltaY);

    reloadViewMatrix();
  }

  g_cursorX = x;
  g_cursorY = y;
}

void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods) {
  using namespace openGL::scene;

  bool set = action != GLFW_RELEASE && GLFW_REPEAT;
  switch (key) {
  case GLFW_KEY_ESCAPE:
    glfwSetWindowShouldClose(window, GL_TRUE);
    break;

  case GLFW_KEY_1:
    cameraMode = 1;
    g_camera.m_pos = math::Vec3f{0.0, 2.0f, 7.0f};
    g_camera.m_forward = math::Vec3f{0.0, 0.0f, -1.0f};
    g_camera.m_up = math::Vec3f{0.0, 1.0f, 0.0f};;
    reloadViewMatrix();
    updateCamera();
    break;

  case GLFW_KEY_2:
    cameraMode = 2;
    updateCamera();
    break;

  case GLFW_KEY_3:
    cameraMode = 3;
    updateCamera();
    break;

  case GLFW_KEY_4:
      std::cout << "Play Sound" << std::endl;
      engine->play2D("external/Scream4.wav", true);
    break;

  case GLFW_KEY_W:
    g_cameraUpdate.set(CameraUpdate::moveForward, set);
    break;
  case GLFW_KEY_S:
    if (mods == GLFW_MOD_CONTROL)
      g_play = false;
    else
      g_cameraUpdate.set(CameraUpdate::moveBackward, set);
    break;
  case GLFW_KEY_A:
    g_cameraUpdate.set(CameraUpdate::moveLeft, set);
    break;
  case GLFW_KEY_D:
    g_cameraUpdate.set(CameraUpdate::moveRight, set);
    break;
  case GLFW_KEY_Q:
    g_cameraUpdate.set(CameraUpdate::moveDown, set);
    break;
  case GLFW_KEY_E:
    g_cameraUpdate.set(CameraUpdate::moveUp, set);
    break;
  case GLFW_KEY_UP:
    g_cameraUpdate.set(CameraUpdate::rotateUp, set);
    break;
  case GLFW_KEY_DOWN:
    g_cameraUpdate.set(CameraUpdate::rotateDown, set);
    break;
  case GLFW_KEY_LEFT:
    if (mods == GLFW_MOD_SHIFT)
      g_cameraUpdate.set(CameraUpdate::rollLeft, set);
    else
      g_cameraUpdate.set(CameraUpdate::rotateLeft, set);
    break;
  case GLFW_KEY_RIGHT:
    if (mods == GLFW_MOD_SHIFT)
      g_cameraUpdate.set(CameraUpdate::rollRight, set);
    else
      g_cameraUpdate.set(CameraUpdate::rotateRight, set);
    break;
  case GLFW_KEY_SPACE:
    g_play = set ? !g_play : g_play;
    break;
  case GLFW_KEY_T:
    if (set) {
      if (--g_numberOfSubdivisions < 0)
        g_numberOfSubdivisions = 0;
      loadCurveGeometryToGPU(g_numberOfSubdivisions);
    }
    break;
  case GLFW_KEY_Y:
    if (set) {
      ++g_numberOfSubdivisions;
      loadCurveGeometryToGPU(g_numberOfSubdivisions);
    }
    break;
  case GLFW_KEY_R:
    if (mods == GLFW_MOD_CONTROL)
      g_play = true;
    break;
  case GLFW_KEY_F:
    if (mods == GLFW_MOD_CONTROL && set)
      oncePerFrame();
    break;
  case GLFW_KEY_P:
    if (mods == GLFW_MOD_CONTROL)
      if (!reloadShadersFromFile())
        std::cerr << "ERROR: shaders could were not read correctly\n";
    break;
  case GLFW_KEY_LEFT_BRACKET:
    if (mods == GLFW_MOD_SHIFT) {
      g_rotationSpeed *= 0.5;
    } else {
      g_panningSpeed *= 0.5;
    }
    break;
  case GLFW_KEY_RIGHT_BRACKET:
    if (mods == GLFW_MOD_SHIFT) {
      g_rotationSpeed *= 1.5;
    } else {
      g_panningSpeed *= 1.5;
    }
    break;
  default:
    break;
  }
}

//==================== OPENGL HELPER FUNCTIONS ====================//

void moveCamera() {
  using namespace openGL::scene;
  if (g_cameraUpdate.needsUpdating()) {
    if (g_cameraUpdate.isSet(CameraUpdate::moveBackward)) {
      g_camera.moveBackward(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::moveForward)) {
      g_camera.moveForward(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::moveUp)) {
      g_camera.moveUp(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::moveDown)) {
      g_camera.moveDown(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::moveLeft)) {
      g_camera.moveLeft(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::moveRight)) {
      g_camera.moveRight(g_panningSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rotateLeft)) {
      g_camera.rotateLeft(g_rotationSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rotateRight)) {
      g_camera.rotateRight(g_rotationSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rotateUp)) {
      g_camera.rotateUp(g_rotationSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rotateDown)) {
      g_camera.rotateDown(g_rotationSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rollLeft)) {
      g_camera.rollLeft(g_rotationSpeed);
    }
    if (g_cameraUpdate.isSet(CameraUpdate::rollRight)) {
      g_camera.rollRight(g_rotationSpeed);
    }

    reloadViewMatrix();
    // g_cameraUpdate.reset(); // reseting seems jittery, so don't
  }
}

void resetCamera() {
  g_camera = openGL::scene::Camera(math::Vec3f{0.f, 0.f, 5.f},
                                   math::Vec3f{0.f, 0.f, -1.f},
                                   math::Vec3f{0.f, 1.f, 0.f});
}

std::string GL_ERROR() {
  GLenum code = glGetError();

  switch (code) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "Non Valid Error Code";
  }
}
