#ifndef BOX_H
#define BOX_H

#include "object.h"
#include <iostream>
#include <vector>

// axis-aligned bounding box object 
class BoundingBox{
    public:
        bool intersect(Ray* ray);
        float boundMin [3] = {};
        float boundMax [3] = {};
        float center [3] = {}; //the center point of the box

        BoundingBox(float min [3], float max [3]);
        
        Object* childObject; //objects that the bounding box contains (can be one)

        std::vector<BoundingBox*> childBoxes; //bounding boxes within this box-- box either will contain an object or boxes

        void addBox(BoundingBox* box);

};

#endif