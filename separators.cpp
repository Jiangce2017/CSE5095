


// Utilities and System Includes
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <cmath>

// Create a structure for point coordinates (we only use here Cartesian coordinates). W is the (d+1) coordinate and for now d=2
typedef struct _point{
	virtual _point& operator=(const _point &a) // Overload the EQUAL operator for points
	{ 
		x = a.x; y = a.y; w = a.w;
		return *this; 
	};
	double x,y,w;
} sPoint;

// Function generatePoints()
// Returns a set of N points in the d-dimensional space. N is also randomly selected
bool generatePoints(sPoint **pOut, int &iCount)
{
	// Let there be points!
	srand((int) time(NULL)); //Initialize random number generator
	iCount = (unsigned)(rand() % 1000 + 1);
	//std::cout << "Number of points generated: " << iCount << "\n"; //For debugging

	//Limits of the d-space
	int ixL = -100;
	int ixU = 100;
	int iyL = -100;
	int iyU = 100;

	//Allocate the list of points and give random coordinate values
	*pOut = new sPoint[iCount];
	sPoint pntCurrent;
	for (unsigned i=0; i<iCount; i++)
	{
		pntCurrent.x = (rand() % (ixU - ixL) + 1)+ixL;
		pntCurrent.y = (rand() % (iyU - iyL) + 1)+iyL;
		// The points are automatically lifted onto the paraboloid: p_(d+1) = [p_d,(norm(p_d)^2]'
		pntCurrent.w = pntCurrent.x * pntCurrent.x + pntCurrent.y * pntCurrent.y;
		(*pOut)[i] = pntCurrent;
	}
	//Output the points for debugging
	//for (unsigned i=0; i<iCount; i++)
	//	std::cout << "Point: " << i << " of [" << (*pOut)[i].x << "," << (*pOut)[i].y << "," << (*pOut)[i].w << "]\n";

	// ToDo:
	// Create error catch for various stuff that might happen
	return true;
};

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    printf("[GeometricSeparators] - Starting...\n");
	sPoint *points = NULL;
	int N;
	generatePoints(&points,N);

	delete [] points; points = NULL;
	//std::cout << "Number of points ge: " << N << "\n";

	int n; 	std::cin >> std::hex >> n;
}