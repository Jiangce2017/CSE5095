
//Application includes
#include "utils.h"
#include "rnd.h"
#include "centerpoint.h"

// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>

#define GLEW_STATIC // Depending on how you built/installed GLEW, you may want to change this
#include <glew-1.9.0\include\GL\glew.h> //Always include GLEW before GLFW or GLM
#include <glfw-2.7.6\include\GL\glfw.h> //Toolkit
#include <glm-0.9.4.0\glm\glm.hpp> //Matrix manipulations
#include <glm-0.9.4.0\glm\gtc\matrix_transform.hpp> //View transforms
#include <glm-0.9.4.0\glm\gtc\quaternion.hpp> //Quaternions operations

using namespace glm;

void runRenderLoop(void);
bool sPoint2Array(sPoint const*, unsigned, GLfloat **);
void DrawCircle(float, float, float, int, GLfloat**);

void Initialize(int, char*[]);
void InitWindow(int, char*[]);


void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);