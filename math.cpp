#include "math.h"

float dotProduct(float a [3], float b [3])
{
	float result = 0;
	for(int i = 0; i < 3; i++)
	{
		result = result + a[i]*b[i];
	}
	return result;
}

//get the cross product between a and b, return in array r
void crossProduct(float a[3], float b [3], float r [3]){
	r[X_AXIS] = a[Y_AXIS]*b[Z_AXIS] - a[Z_AXIS]*b[Y_AXIS];
	r[Y_AXIS] = a[Z_AXIS]*b[X_AXIS] - a[X_AXIS]*b[Z_AXIS];
	r[Z_AXIS] = a[X_AXIS]*b[Y_AXIS] - a[Y_AXIS]*b[X_AXIS];
}
