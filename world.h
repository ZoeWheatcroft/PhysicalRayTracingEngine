#ifndef WORLD_H
#define WORLD_H
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "object.h"
#include "camera.h"
#include "math.h"

class World{
    public:
        void test();

        // all the objects in the world
        std::vector<Object*> objects;
        // all the lights in the world
        std::vector<Light*> lights;
        //the camera for the world
        Camera* camera;

        // add an object to world's list of objs
        //return 0 on success
        int addObject(Object* obj);

        //add a light to world's list of lights
        //return 0 on success
        int addLight(Light* light);

        //get color based on light and intersection info
        //use phong model to calculate color from light
        //return 0 on success
        int applyPhong(IntersectionInfo* info, Color* color);

        void getReflectionVector(IntersectionInfo* info, Ray incomingRay, Ray* reflectionRay);

        //ambient light represented as a color
        //255,255,255 is pure white light
        //0,0,0 is no light
        Color* ambientLight;

        World();

        ~World();
};

#endif