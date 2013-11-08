


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
	char k = 3; //Multiplier for number of points
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
		printf("P[%d] = {%f\t%f\t%f}\n",i,(*pOut)[i].x,(*pOut)[i].y,(*pOut)[i].w);

	// ToDo:
	// Create error catch for various stuff that might happen
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

// Function solveLinSys(sPoint *pntIn, sPoint &pntOut): solves by Gauss-ellimination a linear system of equations
bool solveLinSys(sPoint *set, sPoint &pntOut)
{
	// This two lines are for hystorical purpose - the first time this function worked :)
	// double sys[4][5] = {{1,2,-3,4,5},{1,-5,6,4,-2},{-3,5,-1,4,6},{1,1,1,1,1}};
	// Result should be: {0.028133;0.585678;-0.322251;0.708440}

	//create input (for debugging)
	//sPoint *set = new sPoint[5];

	// Create a copy of the input - the Augmented matrix
	double **aug = new double*[4];
	for (char i=0; i<4; i++)
		aug[i] = new double[5];

	// Populate the input set and copy into augmented matrix (for debugging)
	for (char i=0; i<5; i++)
	{
		//set[i].x = (rand() %100 + 1)/15.0; //This three lines will be commented-out during normal operation
		//set[i].y = (rand() %100 + 1)/11.0;
		//set[i].w = (rand() %100 + 1)/19.0;
		aug[0][i] = set[i].x; aug[1][i] = set[i].y; aug[2][i] = set[i].w; aug[3][i] = 1.0; 
	}

	// Printout Augmented matrix (for debugging)
	//std::cout << "Augmented matrix" << std::endl;
	//for (char i=0; i<4; i++){
	//	for (char j=0; j<5; j++){
	//		printf("%f\t",aug[i][j]);
	//	}
	//	std::cout << std::endl; }
	//std::cout << std::endl << std::endl;

	//Build row-echelon matrix (upper triangular) starting from lower-left and then go up and to the right
	for (char k=0; k<4; k++){ 
		for (char i=3; i>k; i--){
			double dPivL = aug[i][k]; //Store pivot because cell value will get changed
			double dPivU = aug[i-1][k]; //This store is redundant but hey, it's just one double
			for (char j=k; j<5; j++){
				aug[i][j]   = aug[i][j]*dPivU; //Multiply entire row by Upper pivot
				aug[i-1][j] = aug[i-1][j]*dPivL; //Multiply entire row by Lower pivot
				aug[i][j]   = aug[i][j] - aug[i-1][j]; //Subtract line above from curent line, which guarantes element aug[i][k]=0
			}
		}
	}

	// Printout Row-echelon matrix (for debugging)
	//for (char i=0; i<4; i++){
	//	for (char j=0; j<5; j++){
	//		printf("%f\t",aug[i][j]);
	//	}
	//	std::cout << std::endl;	}

	// Calculate alphas (this is actually the equation solver)
	double alpha[5] = {0,0,0,0,-1};
	alpha[3] = aug[3][4]/aug[3][3]; // The last solution is trivial: alpha4 = d(4)/c(4,4) (see notes for details), the rest are automatic
	for (char i=2; i>=0; i--){
		double sum = 0.0;
		for (char j=i+1; j<4; j++)
			sum += aug[i][j] * alpha[j];
		alpha[i] = (aug[i][4]-sum)/aug[i][i];
	}

	// Print solution of alphas (for debugging)
	//std::cout << "Solution is:" << std::endl;
	//for (char i=0; i<5; i++)
	//	printf("\t%f\n",result[i]);

	//std::cout << "Verify the alphas are balanced" << std::endl; // Verify visually the alphas are balanced (for debugging)
	// Calculate the normalization factors for positive alphas and negative alphas (they should be equal in absolute value)
	double dPos = 0.0;
	double dNeg = 0.0;
	for (char i=0; i<5; i++){
		(alpha[i] >=0.0) ? dPos += alpha[i] : dNeg += alpha[i];
	}
	//printf("Sum Positive alpha: %f\nSum Negative alpha: %f\n",sumplus,summinus);

	//Calculate the centerpoint
	sPoint pntCenter1; pntCenter1 = 0;
	sPoint pntCenter2; pntCenter2 = 0;
	for (char i=0; i<5; i++){
		(alpha[i]>=0.0) ? pntCenter1 += set[i] * (alpha[i]/dPos) : pntCenter2 += set[i] * (alpha[i]/dNeg);
	}

	// Output the value of the centerpoint (for debugging)
	printf("Local CenterPnt: [%f\t%f\t%f]\n",pntCenter1.x,pntCenter1.y,pntCenter1.w);
	//printf("Center 2: [%f,%f,%f]\n",pntCenter2.x,pntCenter2.y,pntCenter2.w);

	//ToDo: make an ASSERT function which checks if indeed Center1=Center2
	pntOut = pntCenter1; //Return result


	// Clean-up the augmented matrix and the input set (input set is used only for debugging)
	for (char i=0; i<4; i++){
		delete [] aug[i];
	}
	delete [] aug; aug = NULL;
	//delete [] set; set = NULL;

	// Return from function
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
		//To Do: Process special cases of 2,3,4 points for when total number of points is not a k-multiplier of 5
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

	// This calculates the centroid
	//pntOut.x = 0.2*(ppntCenter[0].x+ppntCenter[1].x+ppntCenter[2].x+ppntCenter[3].x+ppntCenter[4].x);
	//pntOut.y = 0.2*(ppntCenter[0].y+ppntCenter[1].y+ppntCenter[2].y+ppntCenter[3].y+ppntCenter[4].y);
	//pntOut.w = 0.2*(ppntCenter[0].w+ppntCenter[1].w+ppntCenter[2].w+ppntCenter[3].w+ppntCenter[4].w);

	solveLinSys(ppntCenter,pntOut); //Get the Radon point of the current 5 points

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
	printf("CenterPoint = {%f\t%f\t%f}\n",pntCenter.x,pntCenter.y,pntCenter.w);
	
	int n; 	std::cin >> std::hex >> n;
	delete [] points; points = NULL; //Dont forget to deallocate
}