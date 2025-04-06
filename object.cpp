#include <cmath>

#include "object.h"

float Object::intersect(IntersectionInfo* info, Ray ray)
{
    printf("just an object... no intersection");
}

int Object::getTextureColor(IntersectionInfo *info)
{
    return 0;
}

Object::Object()
{
    //printf("constructed obj"); 
}

Sphere::Sphere(float x, float y, float z, float radius)
{
	center[X_AXIS] = x;
	center[Y_AXIS] = y;
	center[Z_AXIS] = z;

	this->radius = radius;
}

int Sphere::getTextureColor(IntersectionInfo* info)
{
	if((int)this->texture == 0)
	{
		return 0;
	}

	return 0;
}

float Sphere::intersect(IntersectionInfo* info, Ray ray)
{
	info->mat.color = {255,0,0};
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
	info->mat = this->mat;
	info->mat.color = this->mat.color;
	

	float posDist = (-B+sqrt(root))/2;
	float negDist = (-B-sqrt(root))/2;

	//use smallest dist 
	float dist = posDist > negDist ? negDist : posDist;

	//calculate intersection point and store
	info->intersectionLocation[X_AXIS] = ray.origin[X_AXIS] + ray.direction[X_AXIS]*dist;
	info->intersectionLocation[Y_AXIS] = ray.origin[Y_AXIS] + ray.direction[Y_AXIS]*dist;
	info->intersectionLocation[Z_AXIS] = ray.origin[Z_AXIS] + ray.direction[Z_AXIS]*dist;

	//calculate normal and store
	float normal [3] = {};
	for(int i = 0; i < 3; i++)
	{
		normal[i] = info->intersectionLocation[i] - center[i];
	}
	//normalize normal
	float normalMag = sqrt(pow(normal[X_AXIS], 2) + pow(normal[Y_AXIS], 2) + pow(normal[Z_AXIS], 2));
	for(int i = 0; i < 3; i++)
	{
		float x = normal[i]/normalMag;
		normal[i] = normal[i]/normalMag;
	}
	std::copy(std::begin(normal), std::end(normal), info->normal);

	return dist;

}


float Triangle::intersect(IntersectionInfo* info, Ray ray)
{
	info->mat.color = {255,0,0};

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

	info->mat = this->mat;
	info->mat.color = this->mat.color;
	
	//calculate intersection point and store
	info->intersectionLocation[X_AXIS] = ray.origin[X_AXIS] + ray.direction[X_AXIS]*w;
	info->intersectionLocation[Y_AXIS] = ray.origin[Y_AXIS] + ray.direction[Y_AXIS]*w;
	info->intersectionLocation[Z_AXIS] = ray.origin[Z_AXIS] + ray.direction[Z_AXIS]*w;
	
	//TODO normal calculation
	info->normal[0] = 0;
	info->normal[1] = 1;
	info->normal[2] = 0;

	return w;
	            
}

int Triangle::getTextureColor(IntersectionInfo *info)
{
	if((int)this->texture == 0)
	{
		//if texture is none, return base color
		return 0;
	}

	//TODO: add rotation to get flattened coords first
	//note: in order for two checkerboards to align, their p0s need to be at the same z 
	//		this is because checker size is not fitted to z length, so can overflow (board can have 2.5 sq in y direction)
	if(this->texture == TextureEnum::CHECKER)
	{
		//get checker color from position
		//checker assumes that 0 is the 90 degree corner of the triangle

		//we'll set the checker squares to be n*n size, where n is x width/3 on p1->p2
		float checkerWidth = 0;
		if(point0[X_AXIS] != point1[X_AXIS]){
			checkerWidth = fabs(point0[X_AXIS] - point1[X_AXIS]);
		}
		else{
			checkerWidth = fabs(point0[X_AXIS] - point2[X_AXIS]);
		}
		checkerWidth = checkerWidth/15;

		//check u,v position against width, where 0,0 in local coords is point0
		//0,0 starts with a red square
		float x = info->intersectionLocation[X_AXIS];
		float z = info->intersectionLocation[Z_AXIS];

		//if difference between x and point0[x] / checker width % 2 == 0 (even), red
		int xMod = (int)std::floor(std::fabs(x - point0[X_AXIS])/checkerWidth)%2;
		int zMod = (int)std::floor(std::fabs(z - point0[Z_AXIS])/checkerWidth)%2;
		if(xMod == zMod)
		{
			info->mat.color = {255, 0, 0};
		}
		else{
			info->mat.color = {255, 255, 0};
		}
	}

    return 0;
}

//this is not a cross product
//this is dot product
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