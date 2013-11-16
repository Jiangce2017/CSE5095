
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

int doInitWnd(void);
int runMainLoop(void);
bool sPoint2Array(sPoint const*, unsigned, GLfloat **);