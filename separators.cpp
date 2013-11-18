


// Utilities and System Includes
#include <iostream>

// Application includes
#include "utils.h"
#include "rnd.h"
#include "centerpoint.h"
#include "GO.h"



////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    printf("[GeometricSeparators] - Starting...\n");
	// Generate the set of points
	//sPoint *points = NULL;
	//unsigned N;
	//generatePoints(&points,N);
	//printf("\nGenerated %d points in R^2\n",N);
	// Calculate their CenterPoint
	//sPoint pntCenter;
	//getCenterPoint(points,N,pntCenter);
	//printf("CenterPoint = {%f\t%f\t%f}\n",pntCenter.x,pntCenter.y,pntCenter.w);

	//Initialize graphic output and open a window
	//doInitWnd();
	Initialize(argc, argv);

	//Register callback for window resize
	glfwSetWindowSizeCallback( WindowResize );

	//Run the GO main loop
	runRenderLoop();
	//RenderFunction();
	
	// Clean-up and terminate Main
	//delete [] points; points = NULL; //Dont forget to deallocate
}