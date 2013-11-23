


// Utilities and System Includes
#include <iostream>

// Application includes
//#include "utils.h"
//#include "rnd.h"
//#include "centerpoint.h"
#include "GO.h"



////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    printf("[GeometricSeparators] - Starting...\n");
	
	//Initialize a window and its OGL context
	Initialize();

	//Run the GO main loop
	runRenderLoop();
	
	printf("[GeometricSeparators] - Exit\n");
}