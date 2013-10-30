


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

	char d = 3; //Dimensionality of points
	char k = 8; //Multiplier for number of points
	iCount = pow((double)(d+2),(int)k); //Or generate a fixed number of points equal to (d+2)^k

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

// Function buildBalancedTree()
// Input : a set of points in P{subset}R^d
// Output : a balanced (d+2)-way tree T, of L leaves (for some integer L)
bool buildBalancedTree(sPoint const* pntP, unsigned n)
{
	// As of 29Oct'13 the code works with points in R^3; thus d=3 => T is a 5-way tree
	return true;
};

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
	unsigned uNewn = n/5;
	//Create 5 groups of points
	sPoint *pntGroup0 = new sPoint[uNewn];
	sPoint *pntGroup1 = new sPoint[uNewn];
	sPoint *pntGroup2 = new sPoint[uNewn];
	sPoint *pntGroup3 = new sPoint[uNewn];
	sPoint *pntGroup4 = new sPoint[uNewn];
	for (unsigned i=0; i<uNewn; i++)
	{
		pntGroup0[i] = pntSet[0*uNewn+i];
		pntGroup1[i] = pntSet[1*uNewn+i];
		pntGroup2[i] = pntSet[2*uNewn+i];
		pntGroup3[i] = pntSet[3*uNewn+i];
		pntGroup4[i] = pntSet[4*uNewn+i];
	}

	sPoint pntCenter0, pntCenter1, pntCenter2, pntCenter3, pntCenter4;
	//Recursivelly call getCenterPoint for these 5 groups
	getCenterPoint(pntGroup0,uNewn,pntCenter0);
	getCenterPoint(pntGroup1,uNewn,pntCenter1);
	getCenterPoint(pntGroup2,uNewn,pntCenter2);
	getCenterPoint(pntGroup3,uNewn,pntCenter3);
	getCenterPoint(pntGroup4,uNewn,pntCenter4);
	//Deallocate all groups
	delete [] pntGroup0; pntGroup0 = NULL;
	delete [] pntGroup1; pntGroup1 = NULL;
	delete [] pntGroup2; pntGroup2 = NULL;
	delete [] pntGroup3; pntGroup3 = NULL;
	delete [] pntGroup4; pntGroup4 = NULL;

	// The combine phase of Divide-and-Conquer
	// For now it calculates the centroid but with tweeks it will compute the centerpoint
	pntOut.x = 0.2*(pntCenter0.x+pntCenter1.x+pntCenter2.x+pntCenter3.x+pntCenter4.x);
	pntOut.y = 0.2*(pntCenter0.y+pntCenter1.y+pntCenter2.y+pntCenter3.y+pntCenter4.y);
	pntOut.w = 0.2*(pntCenter0.w+pntCenter1.w+pntCenter2.w+pntCenter3.w+pntCenter4.w);
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
	std::cin >> std::hex >> n;
}