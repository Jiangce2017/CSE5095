/////////////////////////////////
//////////// rnd.cpp ////////////
/////////////////////////////////

// Module which generates randomly a cloud of points in 2D and then automatically lifts them onto a paraboloid
// using the mapping w = x^2 + y^2; nativelly, the code works with points in 2D + 1 dimensions.
// 

#include "rnd.h"

// Function generatePoints(). Returns a set of N points in the d-dimensional space.
bool generatePoints(sPoint **pOut, unsigned &iCount)
{
	// Let there be points!
	srand((int) time(NULL)); //Initialize random number generator
	//iCount = (unsigned)(rand() % 1000 + 1); //Generate random number of points (at most 1000)

	char d = 3; //Dimensionality of points (in the projection to the paraboloid)
	char k = 6; //Multiplier for number of points
	iCount = (unsigned) pow((double)(d+2),k); //Or generate a fixed number of points equal to (d+2)^k

	//Limits of the d-space
	int ixL = -100;
	int ixU =  100;
	int iyL = -100;
	int iyU =  100;
	int irangeX = ixU - ixL;
	int irangeY = iyU - iyL;

	//Allocate the list of points and give random coordinate values
	*pOut = new sPoint[iCount];
	sPoint pntCurrent;
	for (unsigned i=0; i<iCount; i++)
	{
		//1st strategy of generating random points
		//pntCurrent.x = (rand() % irangeX + 1) + ixL;
		//pntCurrent.y = (rand() % irangeY + 1) + iyL;

		//2nd strategy of generating random points
		//pntCurrent.x = ((rand() % irangeX + 1) + ixL)/ixU;
		//pntCurrent.y = ((rand() % irangeY + 1) + iyL)/iyU;
		
		//3rd strategy of generating random points
		double rand1 = (rand() % irangeX + 1) + ixL;
		double rand2 = (rand() % irangeX + 1) + ixL;
		(rand2 != 0.0f) ? pntCurrent.x = rand1/rand2 : pntCurrent.x = rand1/3.1415; //x-coord as a ratio of two random numbers
		rand1 = (rand() % irangeY + 1) + iyL;
		rand2 = (rand() % irangeY + 1) + iyL;
		(rand2 != 0.0f) ? pntCurrent.y = rand1/rand2 : pntCurrent.y = rand1/3.1415; //y-coord as a ratio of two random numbers

		// The points are automatically lifted onto the paraboloid: p_(d+1) = [p_d,(norm(p_d)^2]'
		pntCurrent.w = pntCurrent.x * pntCurrent.x + pntCurrent.y * pntCurrent.y;
		(*pOut)[i] = pntCurrent;
	}
	//Output the points for debugging
	//for (unsigned i=0; i<iCount; i++)
	//	printf("P[%d] = {%f\t%f\t%f}\n",i,(*pOut)[i].x,(*pOut)[i].y,(*pOut)[i].w);

	// ToDo:
	// Create error catch for various stuff that might happen
	return true;
};