


// Utilities and System Includes
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <cmath>

// Application includes
#include "utils.h"
#include "Tree.h"


// Function generatePoints()
// Returns a set of N points in the d-dimensional space. N is also randomly selected
bool generatePoints(sPoint **pOut, unsigned &iCount)
{
	// Let there be points!
	srand((int) time(NULL)); //Initialize random number generator
	//iCount = (unsigned)(rand() % 1000 + 1); //Generate random number of points (at most 1000)

	char d = 3; //Dimensionality of points (in the projection to the paraboloid)
	char k = 1; //Multiplier for number of points
	iCount = (unsigned) pow((double)(d+2),k); //Or generate a fixed number of points equal to (d+2)^k

	//Limits of the d-space
	int ixL = -100;
	int ixU = 100;
	int iyL = -100;
	int iyU = 100;
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
	for (unsigned i=0; i<iCount; i++)
		printf("P[%d] = {%f,%f,%f}\n",i,(*pOut)[i].x,(*pOut)[i].y,(*pOut)[i].w);

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

// Function buildBalancedTree()
// Input : a set of points in P{subset}R^d
// Output : a balanced (d+2)-way tree T, of L leaves (for some integer L)
bool buildBalancedTree(sPoint const* pntP, unsigned n)
{
	// As of 29Oct'13 the code works with points in R^3; thus d=3 => T is a 5-way tree
	return true;
};

// Function getCenter5()
// Input: 5 points in R^3 (in general position)
// Output: centerpoint (a Radon point)
bool getCenter5(sPoint const* pntIn, sPoint &pntOut)
{
	// There's no error catch if number of points passed is different than 5 so it better be the right number
	// There can be two cases (assuming general position):
	//		1. One of the points is inside the triangular pyramid formed by the other 4 - then return the inside point
	//		2. The 5 points describe a hexahedron (triangular dipyramid) - then the result is any of the points
	return true;
}

// Function getCenterPoint()
// Calculates the Center Point of a point set using the Iterated-Radon algorithm (recursive)
bool getCenterPoint(sPoint const* pntSet, int n, sPoint &pntOut)
{
	if (n<5)
	{
		//Recursion doesn't apply for this number of points anymore. 
		pntOut = pntSet[0];
		return true;
		//To Do: Process special cases of 1,2,3,4,5 points for when total number of points is not a k-multiplier of 5
	}
	
	//Divide the pointset into 5 balanced! groups
	unsigned uNewn = n/5; //n for now is a multiplier of 5 so division is always .0000

	//Create 5 groups of points and populate with points from the input set
	sPoint **pntGroup = new sPoint*[5];
	sPoint *ppntCenter = new sPoint[5];
	for (char i=0; i<5; i++)
	{
		pntGroup[i] = new sPoint[uNewn];
		for (unsigned j=0; j<uNewn; j++)
			pntGroup[i][j] = pntSet[i*uNewn+j];
		//Recursivelly call getCenterPoint for these 5 groups
		getCenterPoint(pntGroup[i],uNewn,ppntCenter[i]);
		//When done dispose of the current group (out of 5)
		delete [] pntGroup[i];
	}
	//Deallocate all 5 groups
	delete [] pntGroup; pntGroup = NULL;

	// The combine phase of Divide-and-Conquer
	// For now it calculates the centroid but with tweeks it will compute the centerpoint
	pntOut.x = 0.2*(ppntCenter[0].x+ppntCenter[1].x+ppntCenter[2].x+ppntCenter[3].x+ppntCenter[4].x);
	pntOut.y = 0.2*(ppntCenter[0].y+ppntCenter[1].y+ppntCenter[2].y+ppntCenter[3].y+ppntCenter[4].y);
	pntOut.w = 0.2*(ppntCenter[0].w+ppntCenter[1].w+ppntCenter[2].w+ppntCenter[3].w+ppntCenter[4].w);

	//Deallocate current 5 center points
	delete [] ppntCenter; ppntCenter = NULL;
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