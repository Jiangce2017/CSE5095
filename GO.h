
//Application includes
#include "utils.h"
#include "rnd.h"
#include "centerpoint.h"

// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>

//#define GLEW_STATIC // Depending on how you built/installed GLEW, you may want to change this
//#define GLEW_BUILD

//#include <glew-1.9.0\include\GL\glew.h> //Always include GLEW before GLFW or GLM
#include <glew-1.10.0\include\GL\glew.h>
#include <glfw-2.7.6\include\GL\glfw.h> //Toolkit
#include <glm-0.9.4.0\glm\glm.hpp> //Matrix manipulations
#include <glm-0.9.4.0\glm\gtc\matrix_transform.hpp> //View transforms
#include <glm-0.9.4.0\glm\gtc\quaternion.hpp> //Quaternions operations

using namespace glm;

void runRenderLoop(void);
//bool sPoint2Array(sPoint const*, unsigned, GLfloat **);
bool sPoint2VertexArray(sPoint const* InputPoints, unsigned NbrInputPoints, GLfloat **OutputArray);
bool sPoint2ColourArray(sPoint const* InputPoints, unsigned NbrInputPoints, GLfloat **OutputArray);
void DrawCircle(float, float, float, int, GLfloat**);

void Initialize(void);
void InitWindow(void);

void updateView(void);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);