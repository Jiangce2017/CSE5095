


// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>

#define GLEW_STATIC // Depending on how you built/installed GLEW, you may want to change this
#include <glew.h>// Include GLEW. Always include it before gl.h and glfw.h (magic, don't ask)
//#include <gl\GL.h>

#include <glfw.h>// Include GLFW

#include <glm.hpp>// Include GLM
using namespace glm;

#include <shader.hpp>// Include a shader

int doInitWnd(void);
int runMainLoop(void);