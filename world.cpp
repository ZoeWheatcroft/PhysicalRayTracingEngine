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
    *ambientLight = {25, 25, 25};;
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
        float incomingLightDir [3] = {light->center[X_AXIS] - intersection[X_AXIS], light->center[Y_AXIS]-intersection[Y_AXIS] , light->center[Z_AXIS]-intersection[Z_AXIS]};

        //normalize the direction
        float incomingMag = sqrt(pow(incomingLightDir[X_AXIS], 2) + pow(incomingLightDir[Y_AXIS], 2) + pow(incomingLightDir[Z_AXIS], 2));
        for(int i = 0; i < 3; i++)
        {
            incomingLightDir[i] = incomingLightDir[i]/incomingMag;
        }

        float length = sqrt(pow(incomingLightDir[X_AXIS], 2) + pow(incomingLightDir[Y_AXIS], 2) + pow(incomingLightDir[Z_AXIS], 2));
        if(length < 0.999 || length > 1.001 )
        {
            printf("INCORRECT NORMALIZATION");
        }

        float normal_incoming_cross = dotProduct(incomingLightDir, info->normal);
        if(normal_incoming_cross < 0)
        {
            normal_incoming_cross = 0;
        }
        normal_incoming_cross = normal_incoming_cross * info->mat.kD;
        Color* color = new Color();
        *color = light->mat.color;
        color->red = color->red*normal_incoming_cross;
        color->green = color->green*normal_incoming_cross;
        color->blue = color->blue*normal_incoming_cross;

        L->add(color);

        //calculate refelction vector
        float reflectionVector [3] = {};
        
        float normalLength = sqrt(pow(info->normal[X_AXIS], 2) + pow(info->normal[Y_AXIS], 2) + pow(info->normal[Z_AXIS], 2));
        float temp = 2*dotProduct(incomingLightDir, info->normal)/pow(normalLength, 2);
        std::copy(std::begin(info->normal), std::end(info->normal), reflectionVector);
        for(int i = 0; i < 3; i++)
        {
            reflectionVector[i] = reflectionVector[i] * temp;
            reflectionVector[i] = incomingLightDir[i] - reflectionVector[i];
        }
        
        //reverse of that
        float viewingDir [3] = {};
        std::copy(std::begin(camera->viewpoint), std::end(camera->viewpoint), viewingDir);
        for(int i = 0; i < 3; i++){
            viewingDir[i] -= info->intersectionLocation[i];
        }
        
        //normalize viewing direction
        float viewingMag = sqrt(pow(viewingDir[X_AXIS], 2) + pow(viewingDir[Y_AXIS], 2) + pow(viewingDir[Z_AXIS], 2));
        for(int i = 0; i < 3; i++)
        {
            viewingDir[i] = viewingDir[i]/viewingMag;
        }

        
        // to kE as exponent
        float specularScalar = pow(dotProduct(reflectionVector, viewingDir), info->mat.kE);
        specularScalar = specularScalar * info->mat.kS;
        Color* specColor = new Color();
        *specColor = light->mat.color;
        specColor->red = specColor->red*specularScalar;
        specColor->green = specColor->green*specularScalar;
        specColor->blue = specColor->blue*specularScalar;

        //add diffuse
        L->add(specColor);

        if(L->red < 0 || L->blue < 0 || L->green < 0)
        {
            printf("\nnegative color error");
        }
    }

    //tone reproduction

}


void World::getReflectionVector(IntersectionInfo* info, Ray incomingRay, Ray* reflectionRay)
{
    //calculate refelction vector

    float normalLength = sqrt(pow(info->normal[X_AXIS], 2) + pow(info->normal[Y_AXIS], 2) + pow(info->normal[Z_AXIS], 2));
    // float temp = 2*dotProduct(reflectionRay.direction, info->normal)/pow(normalLength, 2);
    float temp = dotProduct(incomingRay.direction, info->normal);
    temp = temp / pow(normalLength, 2);
    for(int i = 0; i < 3; i++)
    {
        reflectionRay->direction[i] = info->normal[i] * temp;
        reflectionRay->direction[i] = reflectionRay->direction[i]*2;
        reflectionRay->direction[i] = incomingRay.direction[i] - reflectionRay->direction[i];
    }

    std::copy(std::begin(info->intersectionLocation), std::end(info->intersectionLocation), reflectionRay->origin);

    //normalize vector 

    float magnitude = sqrt(pow(reflectionRay->direction[X_AXIS], 2) + pow(reflectionRay->direction[Y_AXIS], 2) + pow(reflectionRay->direction[Z_AXIS], 2));
    for(int i = 0; i < 3; i++)
    {
        reflectionRay->direction[i] = reflectionRay->direction[i]/magnitude;
    }
}