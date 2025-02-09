#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include "color.h"
#include "ray.h"
#include "main.h"

class Object{
    public:
        int material;
        virtual Color intersect();

        Object();

        ~Object();

};


class Sphere : public Object{
    public:
        bool intersect(Color* color, Ray ray);

        float center [3] = {};
        float radius;
        Color color;

        Sphere(float x, float y, float z, float radius);

};

#endif