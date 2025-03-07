#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "color.h"
#include "ray.h"
#include "main.h"

//intersection info: location, normal, color
struct IntersectionInfo{
    float intersectionLocation [3] = {};
    float normal [3] = {};
    Color* color;
};

//enum of texture options for an object
enum class TextureEnum {
	NONE,
	CHECKER,
	IMAGE
	//TODO: brick one day! 
};


class Object{
    public:
        int material;
        //intersection takes a pointer to info(that it fills if applicable) and ray
        //return: distance (w) of intersection or -1
        virtual float intersect(IntersectionInfo* info, Ray ray);

        //given an intersection location, get the color for the texture
        //if there is no texture, nothing should be done and the intersection info should be unchanged
        //if there is a texture, update the color in intersection info based on it. 
        //return: 0 on success, 1 on failure
        virtual int getTextureColor(IntersectionInfo* info);

        Color color;
        enum TextureEnum texture;
        Object();

        ~Object();

};

//in the case of light, its color value is the radiance for each light channel
//ie, 0,0,0 is no light and 255,255,255 is a max white light
class Light : public Object{
    public:
        float intersect(IntersectionInfo* info, Ray ray);
        float center [3] = {};
        float radius;

        Light(float x, float y, float z, float radius);
};

class Sphere : public Object{
    public:
        float intersect(IntersectionInfo* info, Ray ray);
        int getTextureColor(IntersectionInfo* info);
        float center [3] = {};
        float radius;

        Sphere(float x, float y, float z, float radius);

};

class Triangle : public Object{
    public:
        float intersect(IntersectionInfo* info, Ray ray);
        int getTextureColor(IntersectionInfo* info);
        float point0 [3] = {};
        float point1 [3] = {};
        float point2 [3] = {};


        float cross(float a [3], float b [3]);
};



#endif