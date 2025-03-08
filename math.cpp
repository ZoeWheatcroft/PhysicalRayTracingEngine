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
