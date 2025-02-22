#include <cmath>

#include "object.h"

float Object::intersect(IntersectionInfo* info, Ray ray)
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

float Sphere::intersect(IntersectionInfo* info, Ray ray)
{
	Color* color = new Color{0,0,0};
	info->color = color;
	*color = {255, 0, 0};

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

	float root = pow(B, 2) - 4*C;

	if(root < 0)
	{
		return -1;
	}

	//calculate root
	*color = this->color;

	float posDist = (-B+sqrt(root))/2;
	float negDist = (-B-sqrt(root))/2;

	//use smallest dist 
	float dist = posDist > negDist ? negDist : posDist;

	//calculate intersection point and store
	info->intersectionLocation[X_AXIS] = ray.origin[X_AXIS] + ray.direction[X_AXIS]*dist;
	info->intersectionLocation[Y_AXIS] = ray.origin[Y_AXIS] + ray.direction[Y_AXIS]*dist;
	info->intersectionLocation[Z_AXIS] = ray.origin[Z_AXIS] + ray.direction[Z_AXIS]*dist;

	//calculate normal and store
	float normal [3];
	for(int i = 0; i < 3; i++)
	{
		normal[i] = info->intersectionLocation[i] - center[i];
	}
	//normalize normal
	float normalMag = sqrt(pow(normal[X_AXIS], 2) + pow(normal[Y_AXIS], 2) + pow(normal[Z_AXIS], 2));
	for(int i = 0; i < 3; i++)
	{
		normal[i] = normal[i]/normalMag;
	}
	*info->normal = *normal;

	return dist;

}

float Triangle::intersect(IntersectionInfo* info, Ray ray)
{
	Color* color = new Color{0,0,0};
	info->color = color;
	*color = {255, 0, 0};

	
	float edge1 [3] = {0, 0, 0};
	float edge2 [3] = {0, 0, 0};
	float T [3] = {0,0,0};
	float P [3] = {0,0,0};
	float Q [3] = {0,0,0};

	for(int i = 0; i < 3; i++)
	{
		edge1[i] = point1[i] - point0[i];
		edge2[i] = point2[i] - point0[i];
		T[i] = ray.origin[i] - point0[i];
	}

	// P = cross product D x e2
	P[0] = ray.direction[1]*edge2[2] - ray.direction[2]*edge2[1];
	P[1] = ray.direction[2]*edge2[0] - ray.direction[0]*edge2[2];
	P[2] = ray.direction[0]*edge2[1] - ray.direction[1]*edge2[0];

	//cross product T and e1
	Q[0] = T[1]*edge1[2] - T[2]*edge1[1];
	Q[1] = T[2]*edge1[0] - T[0]*edge1[2];
	Q[2] = T[0]*edge1[1] - T[1]*edge1[0];

	//denominator
	float denominator = cross(P, edge1);

	if(denominator == 0)
	{
		return -1;
	}

	float w = cross(Q, edge2)/denominator;
	float u = cross(P, T)/denominator;
	float v = cross(Q, ray.direction)/denominator;

	if(u < 0 || v < 0 || u + v > 1)
	{
		return -1;
	}

	//calculate intersection point and store
	info->intersectionLocation[X_AXIS] = ray.origin[X_AXIS] + ray.direction[X_AXIS]*w;
	info->intersectionLocation[Y_AXIS] = ray.origin[Y_AXIS] + ray.direction[Y_AXIS]*w;
	info->intersectionLocation[Z_AXIS] = ray.origin[Z_AXIS] + ray.direction[Z_AXIS]*w;
	
	info->normal[0] = 0;
	info->normal[1] = 1;
	info->normal[2] = 0;

	return w;
	            
}

float Triangle::cross(float a [3], float b [3])
{
	float result = 0;
	for(int i = 0; i < 3; i++)
	{
		result = result + a[i]*b[i];
	}
	return result;
}

Light::Light(float x, float y, float z, float radius)
{
	center[X_AXIS] = x;
	center[Y_AXIS] = y;
	center[Z_AXIS] = z;

	this->radius = radius;
}

float Light::intersect(IntersectionInfo* info, Ray ray)
{
	return -1;
}