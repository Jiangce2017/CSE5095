


// Utilities and System Includes
//#include <iostream>
#include <stdio.h>
//#include <SDL2-2.0.1\include\SDL.h>
//#include "Graphics.h"
//#include <tchar.h>

// Application includes
//#include "utils.h"
//#include "rnd.h"
//#include "centerpoint.h"
#include "GO.h"



////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)// Works only for GLFW
//int _tmain(int argc, char* argv[])// Works only for SDL2
{
    printf("[GeometricSeparators] - Starting...\n");
	
	////Initialize a window and its OGL context
	Initialize();
	////Run the GO main loop
	runRenderLoop();
	
	printf("[GeometricSeparators] - Exit\n");
}