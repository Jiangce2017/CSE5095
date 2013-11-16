/////////////////////////////////
////////  centerpoint.h  ////////
/////////////////////////////////

// See the centerpoint.CPP file for a complete description of this module

#include "utils.h"
#include <iostream>
#include <cmath>

bool solveLinSys(sPoint*, sPoint&);
bool getCenterPoint(sPoint const*, int, sPoint&);
bool getPartition(sPoint const*, double*, sPoint*);