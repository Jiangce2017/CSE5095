
//Application includes
#include "utils.h"
#include "rnd.h"
#include "centerpoint.h"

// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>

#define GLEW_STATIC // Depending on how you built/installed GLEW, you may want to change this
#include <glew.h>// Include GLEW. Always include it before gl.h and glfw.h (magic, don't ask)
//#include <gl\GL.h>

#include <glfw.h>// Include GLFW

#include <glm.hpp>// Include GLM
#include <.\gtc\matrix_transform.hpp> //View transforms
using namespace glm;

// GLEW libraries, shaders and control sequences I got from the interwebs
#include <shader.hpp>// Include a shader
#include <controls.hpp>

int runRenderLoop(void);
bool sPoint2Array(sPoint const*, unsigned, GLfloat **);
void DrawCircle(float, float, float, int, GLfloat**);

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void GLFWCALL WindowResize( int width, int height ); //Window resize function
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);