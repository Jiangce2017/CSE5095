/////////////////////////////////
//////// centerpoint.cpp ////////
/////////////////////////////////

// Module which calculates the centerpoint of a given set of 3D points.
// Number of points must be a mutiplier of 5. That is N = 5^k, where k is a natural number.
// The implementation recursively splits the set into 5 groups until the current subgroup has only one point (like building a tree) and returns that.
// Then, goes up one level and calculates the Radon point of current 5 points and returns that, and so forth.
// solveLinSys() is the function which calculates the Radon point of 5 points in 3D
// getCenterPoint() is the recursive function that calculates the Radon point of the whole set.

// Use as:
		//sPoint *points = NULL;
		//unsigned N;
		//generatePoints(&points,N); //This will generate the points
			//printf("\nGenerated %d points in R^2\n",N);
		//sPoint pntCenter;
		//getCenterPoint(points,N,pntCenter); //This will calculate the centerpoint
			//printf("CenterPoint = {%f\t%f\t%f}\n",pntCenter.x,pntCenter.y,pntCenter.w);

#include "centerpoint.h"

// Function solveLinSys(sPoint *pntIn, sPoint &pntOut): solves by Gauss-ellimination a linear system of equations
bool solveLinSys(sPoint *set, sPoint &pntOut)
{
	bool debug = false; //Turn on/off the debuggin info

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
	if (debug){
		std::cout << "Augmented matrix" << std::endl;
		for (char i=0; i<4; i++){
			for (char j=0; j<5; j++){
				printf("%f\t",aug[i][j]);
			}
			std::cout << std::endl; }
		std::cout << std::endl << std::endl;
	}

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
	if (debug){
		for (char i=0; i<4; i++){
			for (char j=0; j<5; j++){
				printf("%f\t",aug[i][j]);
			}
			std::cout << std::endl;	
		}
	}

	// Calculate alphas (this is actually the equation solver)
	double alpha[5] = {0,0,0,0,-1};
	alpha[3] = aug[3][4]/aug[3][3]; // The last solution is trivial: alpha3 = d(3)/c(3,3) (see notes for details), the rest are automatic
	for (char i=2; i>=0; i--){
		double sum = 0.0;
		for (char j=i+1; j<4; j++)
			sum += aug[i][j] * alpha[j];
		alpha[i] = (aug[i][4]-sum)/aug[i][i];
	}


	// Print solution of alphas (for debugging)
	if (debug){
		std::cout << "Solution is:" << std::endl;
		for (char i=0; i<5; i++)
			printf("\t%f\n",alpha[i]);
	}

	//std::cout << "Verify the alphas are balanced" << std::endl; // Verify visually the alphas are balanced (for debugging)
	// Calculate the normalization factors for positive alphas and negative alphas (they should be equal in absolute value)
	double dPos = 0.0;
	double dNeg = 0.0;
	for (char i=0; i<5; i++){
		(alpha[i] >=0.0) ? dPos += alpha[i] : dNeg += alpha[i];
	}

	if (debug){
		printf("Sum Positive alpha: %f\nSum Negative alpha: %f\n",dPos,dNeg);
	}

	//Calculate the centerpoint
	sPoint pntCenter1; pntCenter1 = 0;
	sPoint pntCenter2; pntCenter2 = 0;
	for (char i=0; i<5; i++){
		(alpha[i]>=0.0) ? pntCenter1 += set[i] * (alpha[i]/dPos) : pntCenter2 += set[i] * (alpha[i]/dNeg);
	}

	// Output the value of the centerpoint (for debugging)
	if (debug){
		printf("Local CenterPnt: [%f\t%f\t%f]\n",pntCenter1.x,pntCenter1.y,pntCenter1.w);
		printf("Center 2: [%f,%f,%f]\n",pntCenter2.x,pntCenter2.y,pntCenter2.w);
	}

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

	// This calculates the centroid of current 5 points
	//pntOut.x = 0.2*(ppntCenter[0].x+ppntCenter[1].x+ppntCenter[2].x+ppntCenter[3].x+ppntCenter[4].x);
	//pntOut.y = 0.2*(ppntCenter[0].y+ppntCenter[1].y+ppntCenter[2].y+ppntCenter[3].y+ppntCenter[4].y);
	//pntOut.w = 0.2*(ppntCenter[0].w+ppntCenter[1].w+ppntCenter[2].w+ppntCenter[3].w+ppntCenter[4].w);

	//This calculates the Radon point of the current 5 points
	solveLinSys(ppntCenter,pntOut); 

	//Deallocate current 5 center points
	delete [] ppntCenter; ppntCenter = NULL;
	return true;
}