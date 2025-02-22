#include "world.h"

#include <iostream>
#include <typeinfo>

void World::test()
{
    printf("\nWorld is active and up! Big win!");
}

World::World()
{
    ambientLight = new Color();
    *ambientLight = {100, 100, 100};;
}

int World::addObject(Object* obj)
{

    objects.push_back(obj);

    return 0;
}

int World::addLight(Light* light)
{
    lights.push_back(light);
}

//parameters: intersection info and pointer to return luminance
int World::applyPhong(IntersectionInfo* info, Color* L)
{    
    //ambient light first
    L->add(ambientLight);
    
    //add light for each light
    for(int i = 0; i < lights.size(); i++)
    {
        Light* light = lights[i];

        // can we see light from point
        //calculate ray from point to light

        //advance origin point by 0.01 along normal ray so does not hit itself        
        float alteredOrigin [3] = {};
        std::copy(std::begin(info->intersectionLocation), std::end(info->intersectionLocation), std::begin(alteredOrigin));
        for(int i = 0; i < 3; i++)
        {
            float x = 0;
            x = info->normal[i];
            alteredOrigin[i] += x*0.01;
        }

        Ray ray;
        std::copy(std::begin(alteredOrigin), std::end(alteredOrigin), std::begin(ray.origin));
        float dir [3] = {light->center[X_AXIS] - ray.origin[X_AXIS], light->center[Y_AXIS] - ray.origin[Y_AXIS], light->center[Z_AXIS] - ray.origin[Z_AXIS]};

        //normalize the direction
        float magnitude = sqrt(pow(dir[X_AXIS], 2) + pow(dir[Y_AXIS], 2) + pow(dir[Z_AXIS], 2));
        for(int i = 0; i < 3; i++)
        {
            dir[i] = dir[i]/magnitude;
        }
        std::copy(std::begin(dir), std::end(dir), std::begin(ray.direction));

        //check if light is blocked
        IntersectionInfo* lightIntersection = new struct IntersectionInfo;
        int lightBlocked = 0; 
        for(Object* obj : objects)
        {
            IntersectionInfo* lightInfo = new struct IntersectionInfo;
            float dist = obj->intersect(lightInfo, ray);
            if(dist != 1 && dist > 0){
                const char* type = typeid(*obj).name();
                lightBlocked = 1;
                break;
            }
        }
        if(lightBlocked)
        {
            continue;
        }

        //now we know that we can actually see the light!!

        //get dir of s (incoming light direction)

        float intersection [3];
        std::copy(std::begin(intersection), std::end(intersection), std::begin(info->intersectionLocation));
        float incomingLightDir [3] = {intersection[X_AXIS] - light->center[X_AXIS], intersection[Y_AXIS] - light->center[Y_AXIS], intersection[Z_AXIS] - light->center[Z_AXIS]};

        //normalize the direction
        float incomingMag = sqrt(pow(incomingLightDir[X_AXIS], 2) + pow(incomingLightDir[Y_AXIS], 2) + pow(incomingLightDir[Z_AXIS], 2));
        for(int i = 0; i < 3; i++)
        {
            incomingLightDir[i] = incomingLightDir[i]/incomingMag;
        }

        Triangle* tri = new Triangle(); //making triangle for cross function LOL

        float normal_incoming_cross = tri->cross(incomingLightDir, info->normal);
        normal_incoming_cross = normal_incoming_cross * 0.5;
        Color* color = new Color();
        *color = light->color;
        color->red = color->red*normal_incoming_cross;
        color->green = color->green*normal_incoming_cross;
        color->blue = color->blue*normal_incoming_cross;

        L->add(&light->color);
                
    }

    //tone reproduction

}
