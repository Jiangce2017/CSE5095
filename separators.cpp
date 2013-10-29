


// Utilities and System Includes
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <cmath>

// Create a structure for point coordinates (we only use here Cartesian coordinates). W is the (d+1) coordinate and for now d=2
typedef struct _point{
	double x,y,w;
	virtual _point& operator=(const _point &a){  // Overload the EQUAL operator for points
		x = a.x; y = a.y; w = a.w;
		return *this; };
} sPoint;

// Function generatePoints()
// Returns a set of N points in the d-dimensional space. N is also randomly selected
bool generatePoints(sPoint **pOut, unsigned &iCount)
{
	// Let there be points!
	srand((int) time(NULL)); //Initialize random number generator
	iCount = (unsigned)(rand() % 1000 + 1);

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
	//	printf("P[%d] = {%f,%f,%f}\n",i,(*pOut)[i].x,(*pOut)[i].y,(*pOut)[i].w);

	// ToDo:
	// Create error catch for various stuff that might happen
	return true;
};

bool IteratedTverberg(sPoint const* pntS, int n, sPoint &pntO)
{
	if (n <= 2*(3+1)*(3+1)){ // |pntS|<=2(d+1)^2
		pntO = pntS[0]; //Any point in pntS can be centerpoint
		return true; 
	}
	sPoint pntCurrCenter[3+2];//(d+2) centerpoints
	for (char i=1; i<=(3+2); i++){ // FOR i=1 to d+2
		unsigned in = rand() % (n/2) + 1; //Select not more than n/2 points from set pntS
		sPoint *NEWpntSet = new sPoint[in]; //The new list of points (used for the next recursion)
		unsigned *uMask = new unsigned[in]; //Contains the intexes from 'setP' of each point selected (must be unique of course)
		// Create a mask that contains those indexes of pntS that will be copied into NEWpntSet
		for (unsigned j=0; j<in; j++){
			bool bIsIDXGood = true;
			unsigned uCurrIDX;
			do{
				uCurrIDX = rand() % n + 1;
				for(unsigned k=0; k<j; k++){
					if (uCurrIDX == uMask[k]){
						bIsIDXGood = false;
						break; //No point running the inner FOR, index is already used
					} 
				}
			} while(bIsIDXGood != true);
			uMask[j] = uCurrIDX;
		}
		//Now fill the '*NEWpntSet' with actual points
		for (unsigned h=0; h<in; h++){
			NEWpntSet[h] = pntS[uMask[h]];
		}
		//Recursivelly call 'Iterated-Tverberg' for the new pointset
		IteratedTverberg(NEWpntSet,in,pntCurrCenter[i]);
		delete [] NEWpntSet; NEWpntSet = NULL;
		delete [] uMask; uMask = NULL;
		//We are at 'S <- S\proof(P_i)' 
	}
	//ToDO: There's no need to randomly select points from the initial set because the input is randomized.
	//		If the input is not randomized, create a function that shuffles the indexes of the array of points and problem solved - we're back to random points.

	return true;
};

// Function getCenterPoint()
// Calculates the Center Point of a point set using the Iterated-Tverberg algorithm (recursive)
bool getCenterPoint(sPoint const* pntSet, int n, sPoint &pntOut)
{
	IteratedTverberg(pntSet,n,pntOut);
	pntOut = pntSet[5];//Output test
	return true;
};

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    printf("[GeometricSeparators] - Starting...\n");
	// Generate the set of points
	sPoint *points = NULL;
	unsigned N;
	generatePoints(&points,N);
	printf("Generated %d points in R^2\n",N);
	// Calculate their CenterPoint
	sPoint pntCenter;
	getCenterPoint(points,N,pntCenter);
	printf("CenterPoint = {%f,%f,%f}\n",pntCenter.x,pntCenter.y,pntCenter.w);
	
	int n; 	std::cin >> std::hex >> n;
	delete [] points; points = NULL; //Dont forget to deallocate
}