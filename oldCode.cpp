// Application includes
#include "utils.h"

// Utilities and System Includes
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <cmath>

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

// Implementation of Andrew's monotone chain 2D convex hull algorithm.
// Asymptotic complexity: O(n log n).
// Practical performance: 0.5-1.0 seconds for n=1000000 on a 1GHz machine.
//#include <algorithm>
//#include <vector>
//using namespace std;
// 
//typedef int coord_t;         // coordinate type
//typedef long long coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2
// 
//struct Point {
//        coord_t x, y;
// 
//        bool operator <(const Point &p) const {
//                return x < p.x || (x == p.x && y < p.y);
//        }
//};
// 
//// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
//// Returns a positive value, if OAB makes a counter-clockwise turn,
//// negative for clockwise turn, and zero if the points are collinear.
//coord2_t cross(const Point &O, const Point &A, const Point &B)
//{
//        return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
//}
// 
//// Returns a list of points on the convex hull in counter-clockwise order.
//// Note: the last point in the returned list is the same as the first one.
//vector<Point> convex_hull(vector<Point> P)
//{
//        int n = P.size(), k = 0;
//        vector<Point> H(2*n);
// 
//        // Sort points lexicographically
//        sort(P.begin(), P.end());
// 
//        // Build lower hull
//        for (int i = 0; i < n; i++) {
//                while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
//                H[k++] = P[i];
//        }
// 
//        // Build upper hull
//        for (int i = n-2, t = k+1; i >= 0; i--) {
//                while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
//                H[k++] = P[i];
//        }
// 
//        H.resize(k);
//        return H;
//}