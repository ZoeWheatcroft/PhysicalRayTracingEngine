#include <cmath>

#include "object.h"

Color Object::intersect()
{
    printf("just an object... no intersection");
}

Object::Object()
{
    printf("constructed obj"); 
}


Sphere::Sphere(float x, float y, float z, float radius)
{
	center[X_AXIS] = x;
	center[Y_AXIS] = y;
	center[Z_AXIS] = z;

	this->radius = radius;
}

bool Sphere::intersect(Color* color, Ray ray)
{
    float dx = ray.direction[X_AXIS];
    float dy = ray.direction[Y_AXIS];
    float dz = ray.direction[Z_AXIS];

	float xo = ray.origin[X_AXIS];
	float yo = ray.origin[Y_AXIS];
	float zo = ray.origin[Z_AXIS];

	float A = pow(dx, 2) + pow(dy, 2) + pow(dz, 2);
	if(A < 0.99 || A > 1.01)
	{
		printf("ray was not normalized");
	}
	
	float B = 2* ( dx*(xo - center[X_AXIS]) + dy*(yo-center[Y_AXIS]) + dz*(zo - center[Z_AXIS]));
	float C = pow((xo - center[X_AXIS]), 2) + pow(yo - center[Y_AXIS], 2) + pow(zo - center[Z_AXIS], 2) - pow(radius, 2);

	if((pow(B, 2) - 4*C) >= 0)
	{
		*color = this->color;
		return true;
	}

	return false;

}