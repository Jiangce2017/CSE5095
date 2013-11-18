

#pragma once
#ifndef UTILS_H
#define UTILS_H

// Create a structure for point coordinates (we only use here Cartesian coordinates). W is the (d+1) coordinate and for now d=2
typedef struct _point{
	double x,y,w;
	virtual _point& operator=(const _point &a){  // Overload the EQUAL operator for points (Use as p = q)
		x = a.x; y = a.y; w = a.w;
		return *this; 
	};
	virtual _point& operator=(const double &a){  // Overload the EQUAL operator. (I use this for p = 0, the null vector)
		x = a; y = a; w = a;
		return *this; 
	};
	virtual _point& operator+=(const _point &p){ // Overloaded += (use as p += q)
		x = this->x + p.x; y = this->y + p.y; w = this->w + p.w;
		return *this;
	};
	virtual _point& operator-(const _point &p){ //Overloaded unary minus operator for points (use as -p)
		x = -p.x; y = -p.y; w = -p.w;
		return *this;
	};
	virtual _point& operator-(const _point &p)const{ //Overloaded minus operator for points (vector difference) (use as p - q)
		_point result;
		result.x = this->x - p.x;
		result.y = this->y - p.y;
		result.w = this->w - p.w;
		return result;
	};
	virtual _point& operator+(const _point &p)const{ //Overloaded plus operator for points (vector addition) (use as p + q)
		_point result;
		result.x = this->x + p.x;
		result.y = this->y + p.y;
		result.w = this->w + p.w;
		return result;
	};
	virtual _point& operator*(const double &m){ //Scalar multiplier (use as p*a)
		x = this->x * m; y = this->y * m; w = this->w * m;
		return *this;
	};
	virtual _point& operator/(const double &m){ //Scalar multiplier (use as p/a) <- mathematically this is B.S. because math doesn't allow this operation
		x = this->x / m; y = this->y / m; w = this->w / m;
		return *this;
	};
} sPoint;


#endif // UTILS_H