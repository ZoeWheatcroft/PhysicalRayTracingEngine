#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "color.h"
#include "ray.h"
#include "main.h"

class Object{
    public:
        int material;
        virtual float intersect(Color* color, Ray ray);
        Color color;
        Object();

        ~Object();

};


class Sphere : public Object{
    public:
        float intersect(Color* color, Ray ray);

        float center [3] = {};
        float radius;

        Sphere(float x, float y, float z, float radius);

};

class Triangle : public Object{
    public:
        float intersect(Color* color, Ray ray);

        float point0 [3] = {};
        float point1 [3] = {};
        float point2 [3] = {};


        float cross(float a [3], float b [3]);
};

#endif