
//Application includes
#include "utils.h"
#include "rnd.h"
#include "centerpoint.h"

// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>

//#define GLEW_STATIC // Depending on how you built/installed GLEW, you may want to change this
//#define GLEW_BUILD

#include <glew-1.10.0\include\GL\glew.h>
#include <glfw-3.0.3\include\GLFW\glfw3.h>
//#include <glfw-2.7.6\include\GL\glfw.h> //Toolkit
#include <glm-0.9.4.0\glm\glm.hpp> //Matrix manipulations
#include <glm-0.9.4.0\glm\gtc\matrix_transform.hpp> //View transforms

using namespace glm;

void Initialize(void);
void InitWindow(void);
void Cleanup(void);
void CreateVBO(GLuint *NoPoints, GLuint const* NoPointPartition);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);
void runRenderLoop(void);
void UpdateView(void);

bool sPoint2VertexArray(sPoint const* InputPoints, unsigned NbrInputPoints, GLfloat **OutputArray);
bool sPoint2ColourArray(sPoint const* InputPoints, unsigned NbrInputPoints, GLfloat **OutputArray);
bool VA3DtoVA2D(GLfloat const* InputVertexArray, unsigned NumberOfPoints, GLfloat **OutputVertexArray);
void DrawCircle(float, float, float, int, GLfloat**);



