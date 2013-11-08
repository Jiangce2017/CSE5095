


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


// Function cross(a,b): returns the cross product of two vectors a and b
// Usage: 
//		sPoint pntOut = cross(&pntIn[0],&pntIn[1]);
sPoint cross(sPoint const* a, sPoint const* b){
	sPoint result;
	result.x = (a->y * b->w) - (a->w * b->y);
	result.y = -1*(a->x * b->w) + (a->w * b->x);
	result.w = (a->x * b->y) - (a->y * b->x);
	return result;
};

// Function dot(a,b): returns the dot product of two vectors a and b
// Usage: 
//		double d = cross(&pntIn[0],&pntIn[1]);
double dot(sPoint const* a, sPoint const* b){
	return (a->x * b->x) + (a->y * b->y) + (a->w * b->w);
};

// Function norm(a): returns the norm of a vector
double norm(sPoint const* a){
	return sqrt(pow(a->x,2)+pow(a->y,2)+pow(a->w,2));
};

// Function unitNormal(a,b,c): returns the unit normal vector to the plane determine by 3 points a,b,c
// Usage:
//		sPoint nor = unitNormal(&pntIn[0],&pntIn[1],&pntIn[2]);
sPoint unitNormal(sPoint const* a, sPoint const* b, sPoint const* c){
	sPoint B = *b-*a;
	sPoint C = *c-*a;
	sPoint Den = cross(&B,&C);
	double nor = norm(&Den);
	sPoint result;
	(nor == 0.0f) ? result = Den : result = Den/nor;
	return result;
}

// Function dist(n,x0,xi): returns the distance between a point x0 and a plane. xi is a point on that plane and n is the unit normal to that plane
double dist(sPoint const* n, sPoint const* x0, sPoint const* xi){
	sPoint diff = *x0 - *xi;
	return dot(n,&diff);
};

// Function swap(a,b): swaps two points (a,b)
bool swap(sPoint &a, sPoint &b){
	sPoint temp = a;
	a = b;
	b = temp;
	return true;
};

// Function orientPyramid(a,b,c,d): swap points a,b,c,d such that towards the inside, the cross product is always positive and towards outside is always negative
// Usage:
//		orientPyramid(&test);
//		for (unsigned i=0; i<4; i++)
//		{
//			printf("Point %d: [%f,%f,%f]\n",i,test[i].x,test[i].y,test[i].w);
//		}
bool orientPyramid(sPoint **pnt){
	// This is just a bunch of B.S.
	if (dist(&unitNormal(&(*pnt)[1],&(*pnt)[2],&(*pnt)[3]),&(*pnt)[0],&(*pnt)[1]) < 0) swap((*pnt)[2],(*pnt)[3]);
	if (dist(&unitNormal(&(*pnt)[0],&(*pnt)[2],&(*pnt)[3]),&(*pnt)[1],&(*pnt)[0]) < 0) swap((*pnt)[2],(*pnt)[3]);
	if (dist(&unitNormal(&(*pnt)[0],&(*pnt)[1],&(*pnt)[3]),&(*pnt)[2],&(*pnt)[0]) < 0) swap((*pnt)[1],(*pnt)[3]);
	if (dist(&unitNormal(&(*pnt)[0],&(*pnt)[1],&(*pnt)[2]),&(*pnt)[3],&(*pnt)[0]) < 0) swap((*pnt)[1],(*pnt)[2]);
	return true;
};

// Function signedVolume(): returns the sign of the mix product A{dot}(B{cross}C)
// Usage:
//		double d = signedVolume(&pntIn[0],&pntIn[1],&pntIn[2]);
//		printf("Signed volume: [%f]\n",d);
double signedVolume(sPoint const* a, sPoint const* b, sPoint const* c)
{
	//Variant I: return +/-1 (only care about the sign of the signed volume)
	//double result;
	//(dot(a,&cross(b,c)) < 0.0f) ? result = -1 : result = 1;
	//return result;

	//Variant II: return the actual value of the signed volume
	return dot(a,&cross(b,c));
}

// Function solveLinSys(): solves by Gauss-ellimination a linear system of equations
bool solveLinSys(void)
{
	//double sys[4][5] = {{1,2,-3,4,5},{1,-5,6,4,-2},{-3,5,-1,4,6},{1,1,1,1,1}};
	// Result should be: {0.028133;0.585678;-0.322251;0.708440}

	//create input (for debugging)
	double **set = new double*[4];
	for (char i=0; i<4; i++)
		set[i] = new double[5];

	// Create a copy of the input (may not be needed) - the augmented matrix
	double **aug = new double*[4];
	for (char i=0; i<4; i++)
		aug[i] = new double[5];

	// Populate the input set and copy into augmented matrix (for debugging)
	for (char i=0; i<4; i++)
		for (char j=0; j<5; j++){
			if (i == 3) 
				set[i][j] = 1.0;
			else
				set[i][j] = (rand() %100 + 1)/10.0;// = sys[i][j];
			aug[i][j] = set[i][j];
		}

	// Printout augmented matrix (for debugging)
	std::cout << "Augmented matrix" << std::endl;
	for (char i=0; i<4; i++){
		for (char j=0; j<5; j++){
			printf("%f\t",aug[i][j]);
		}
		std::cout << std::endl; }
	std::cout << std::endl << std::endl;

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

	//result
	double result[5] = {0,0,0,0,-1};
	result[3] = aug[3][4]/aug[3][3]; // The last solution is trivial: alpha4 = d(4)/c(4,4) (see notes for details), the rest are automatic
	for (char i=2; i>=0; i--){
		double sum = 0.0;
		for (char j=i+1; j<4; j++)
			sum += aug[i][j]*result[j];
		result[i] = (aug[i][4]-sum)/aug[i][i];
	}

	// Print solution of alphas (for debugging)
	//std::cout << "Solution is:" << std::endl;
	//for (char i=0; i<5; i++)
	//	printf("\t%f\n",result[i]);

	// Verify visually the alphas are balanced (for debugging)
	//std::cout << "Verify the alphas are balanced" << std::endl;
	// Calculate the normalization factors for positive alphas and negative alphas (they should be equal in absolute value)
	double dPos = 0.0;
	double dNeg = 0.0;
	for (char i=0; i<5; i++){
		if (result[i] >=0.0)
			dPos += result[i];
		else
			dNeg += result[i];}
	//printf("Sum Positive alpha: %f\nSum Negative alpha: %f\n",sumplus,summinus);

	//Calculate the centerpoint
	sPoint pntCenter1; pntCenter1 = 0;
	sPoint pntCenter2; pntCenter2 = 0;
	for (char i=0; i<5; i++){
		if (result[i]>=0.0){
			pntCenter1.x += set[0][i]*result[i]/dPos;
			pntCenter1.y += set[1][i]*result[i]/dPos;
			pntCenter1.w += set[2][i]*result[i]/dPos;
		}
		else{
			pntCenter2.x += set[0][i]*result[i]/dNeg;
			pntCenter2.y += set[1][i]*result[i]/dNeg;
			pntCenter2.w += set[2][i]*result[i]/dNeg;
		}
	}
	printf("Center 1: [%f,%f,%f]\n",pntCenter1.x,pntCenter1.y,pntCenter1.w);
	printf("Center 2: [%f,%f,%f]\n",pntCenter2.x,pntCenter2.y,pntCenter2.w);


	// Clean-up the augmented matrix and the input set (input set is used only for debugging)
	for (char i=0; i<4; i++){
		delete [] aug[i];
		delete [] set[i];
	}
	delete [] aug; aug = NULL;
	delete [] set; set = NULL;

	// Return from function
	return true;
};

// Function getCenter5()
// Input: 5 points in R^3 (in general position)
// Output: centerpoint (a Radon point)
bool getCenter5(sPoint *pntIn, sPoint &pntOut)
{
	// There's no error catch if number of points passed is different than 5 so it better be the right number
	// There can be two cases (assuming general position):
	//		1. One of the points is inside the triangular pyramid formed by the other 4 - then return the inside point
	//		2. The 5 points describe a hexahedron (triangular dipyramid) - then the result is any of the points
	//pntOut = pntIn[0]; //Test output
	
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
	//pntOut.x = 0.2*(ppntCenter[0].x+ppntCenter[1].x+ppntCenter[2].x+ppntCenter[3].x+ppntCenter[4].x);
	//pntOut.y = 0.2*(ppntCenter[0].y+ppntCenter[1].y+ppntCenter[2].y+ppntCenter[3].y+ppntCenter[4].y);
	//pntOut.w = 0.2*(ppntCenter[0].w+ppntCenter[1].w+ppntCenter[2].w+ppntCenter[3].w+ppntCenter[4].w);

	getCenter5(ppntCenter,pntOut);
	solveLinSys();

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
	//printf("CenterPoint = {%f,%f,%f}\n",pntCenter.x,pntCenter.y,pntCenter.w);
	
	int n; 	std::cin >> std::hex >> n;
	delete [] points; points = NULL; //Dont forget to deallocate
}