
#include "bounding_box.h"

bool BoundingBox::intersect(Ray* ray)
{
    float tmin [3] = {0,0,0};
    float tmax [3] = {0,0,0};

    for(int i = 0; i < 3; i++)
    {
        tmin[i] = (boundMin[i] - ray->origin[i]) / ray->direction[i]; 
        tmax[i] = (boundMax[i] - ray->origin[i]) / ray->direction[i]; 
        if (tmin[i] > tmax[i]) {
            std::swap(tmin[i], tmax[i]);
        }
    }


    if ((tmin[X_AXIS] > tmax[Y_AXIS]) || (tmin[Y_AXIS] > tmax[X_AXIS])) 
        return 0.0; 

    if (tmin[Y_AXIS] > tmin[X_AXIS]) tmin[X_AXIS] = tmin[Y_AXIS]; 
    if (tmax[Y_AXIS] < tmax[X_AXIS]) tmax[X_AXIS] = tmax[Y_AXIS]; 

    if ((tmin[X_AXIS] > tmax[Z_AXIS]) || (tmin[Z_AXIS] > tmax[X_AXIS])) 
        return 0,0; 

    if (tmin[Z_AXIS] > tmin[X_AXIS]) tmin[X_AXIS] = tmin[Z_AXIS]; 
    if (tmax[Z_AXIS] < tmax[X_AXIS]) tmax[X_AXIS] = tmax[Z_AXIS]; 

    return true; 
}


// create a box, where point0 < point 1 and they are opposing corners 
BoundingBox::BoundingBox(float min [3], float max [3])
{
    boundMin[X_AXIS] = min[X_AXIS];
    boundMax[X_AXIS] = max[X_AXIS];

    boundMin[Y_AXIS] = min[Y_AXIS];
    boundMax[Y_AXIS] = max[Y_AXIS];

    boundMin[Z_AXIS] = min[Z_AXIS];
    boundMax[Z_AXIS] = max[Z_AXIS];

    for(int i = 0; i < 3; i++){
        center[i] = (boundMin[i] + boundMax[i])/2;
    }
}

void BoundingBox::addBox(BoundingBox *box)
{
    //printf("\nadded box");
    childBoxes.push_back(box);
}
