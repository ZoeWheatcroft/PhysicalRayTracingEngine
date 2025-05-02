#include "world.h"



void World::test()
{
    printf("\nWorld is active and up! Big win!");
}

void World::boxAllObjects()
{
    //to do-- remove 
    float rootMin [3] ={-FLT_MAX, -FLT_MAX, -FLT_MAX};
    float rootMax [3] ={FLT_MAX, FLT_MAX, FLT_MAX};

    rootBox = new BoundingBox(rootMin, rootMax);

    std::vector<BoundingBox*> boxes;
    float t [3] = {1,2,3};
    BoundingBox* test = new BoundingBox(t, t);
    boxes.push_back(test);

    //construct all boxes 
    for(Object* obj : objects){
        // for each point, find min/max to encapsulate
        float min[3] = {0,0,0};
        float max[3] = {0,0,0};

        if(Triangle* tri = dynamic_cast<Triangle*>(obj)){            
            std::copy(std::begin(tri->point0), std::end(tri->point0), std::begin(min));
            std::copy(std::begin(tri->point0), std::end(tri->point0), std::begin(max));
            for(int i = 0; i < 3; i++){
                if(tri->point1[i] < min[i]){
                    min[i] = tri->point1[i];
                }
                if(tri->point2[i] < min[i])
                {
                    min[i] = tri->point2[i];
                }

                if(tri->point1[i] > max[i]){
                    max[i] = tri->point1[i];
                }
                if(tri->point2[i] > max[i])
                {
                    max[i] = tri->point2[i];
                }
            }
        }
        else if(Sphere* sp = dynamic_cast<Sphere*>(obj)){
            std::copy(std::begin(sp->center), std::end(sp->center), std::begin(min));
            std::copy(std::begin(sp->center), std::end(sp->center), std::begin(max));

            for(int i = 0; i < 3; i++)
            {
                min[i] = sp->center[i] - sp->radius;
                max[i] = sp->center[i] + sp->radius;
            }
        }
        else{
            continue;
        }
        BoundingBox* box = new BoundingBox(min, max);
        box->childObject = obj;
    
        boxes.push_back(box);
    }

    //while boxes in boxes
    rootBox = recursiveBuildBVH(boxes, 0);
}

/**
 * 
BoundingBox* World::recursiveBuildBVH(std::vector<BoundingBox*> boxes, int axis) {
    if (boxes.empty()) return nullptr;
    if (boxes.size() == 1) return boxes[0];

    sort(boxes.begin(), boxes.end(), [axis](BoundingBox* a, BoundingBox* b) {
        return a->boundMin[axis] < b->boundMin[axis];
    });

    size_t mid = boxes.size() / 2;
    float split = boxes[mid]->boundMin[axis];

    std::vector<BoundingBox*> left, right;

    //for anything that straddles the split, we will add it to the left and update the left's max to reflect this
    //this will lead to some overlapping BUT will create a nicer split 
    float left_max = 0;
    for (BoundingBox* box : boxes) {
        if (box->boundMax[axis] <= split) {
            left.push_back(box);
            if(left_max < box->boundMin[axis]){
                left_max = box->boundMax[axis];
            }
        } else if (box->boundMin[axis] >= split) {
            right.push_back(box);
        } else {
            left.push_back(box);
            if(left_max < box->boundMin[axis]){
                left_max = box->boundMin[axis];
            }
        }
    }

    BoundingBox* leftNode = recursiveBuildBVH(left, (axis + 1) % 3);
    BoundingBox* rightNode = recursiveBuildBVH(right, (axis + 1) % 3);

    float newMin[3], newMax[3];

    if(leftNode != nullptr && rightNode != nullptr){
        for (int i = 0; i < 3; ++i) {
            newMin[i] = leftNode->boundMin[axis];
            newMax[i] = std::max(left_max, rightNode->boundMax[i]);
        }
    }
    else if(leftNode != nullptr)
    {
        for (int i = 0; i < 3; ++i) {
            newMin[i] = leftNode->boundMin[i];
            newMax[i] = leftNode->boundMax[i];
        }
    }
    else if (rightNode != nullptr){
        for(int i = 0; i < 3; i++){
            newMin[i] = rightNode->boundMin[i];
            newMax[i] = rightNode->boundMax[i];
        }
    }

    BoundingBox* parent = new BoundingBox(newMin, newMax);
    if (leftNode) parent->addBox(leftNode);
    if (rightNode) parent->addBox(rightNode);

    return parent;
}  
*/

BoundingBox* World::recursiveBuildBVH(std::vector<BoundingBox*> boxes, int axis) {

    std::vector<BoundingBox*> children;
    

    //if more than 2 children, sort those children into boxes 
    if(boxes.size() > 2){
        int pivot = boxes.size()/2; //where we split the boxes

        //sort the boxes by the axis 
        sort(boxes.begin(), boxes.end(), [axis](BoundingBox* a, BoundingBox* b) {
            return a->center[axis] < b->center[axis];
        });
    
        std::vector<BoundingBox*> minBoxes; 
        std::vector<BoundingBox*> maxBoxes; 
        //get all bottom chunks and put into list 
        for(int i = 0; i < boxes.size(); i++){
            if(i < pivot){
                minBoxes.push_back(boxes[i]);
            }
            else{
                maxBoxes.push_back(boxes[i]);
            }
        }
        
        //if min and max groups are bigger than 1, recurse
        //else, they are just children of current box 
        if(minBoxes.size() > 1){
            BoundingBox* minChildBox = recursiveBuildBVH(minBoxes, axis += 1);
            children.push_back(minChildBox);
        }
        else if (minBoxes.size() == 1){
            children.push_back(minBoxes[0]);
        }

        if(maxBoxes.size() > 1){
            BoundingBox* maxChildBox = recursiveBuildBVH(maxBoxes, axis+=1);
            children.push_back(maxChildBox);
        }
        else if(maxBoxes.size() == 1){
            children.push_back(maxBoxes[0]);
        }
    }
    else
    {
        for(BoundingBox* b : boxes)
        {
            children.push_back(b);
        }
    }

    //find min/max from children 
    //find the min/max of everything
    float min [3] = {};
    float max [3] = {};

    std::copy(std::begin(children[0]->boundMin), std::end(children[0]->boundMin), min);
    std::copy(std::begin(children[0]->boundMax), std::end(children[0]->boundMax), max);

    for(BoundingBox* b : children)
    {
        for(int j = 0; j < 3; j++){
            if(b->boundMin[j] < min[j]){
                min[j] = b->boundMin[j];
            }
            if(b->boundMax[j] > max[j]){
                max[j] = b->boundMax[j];
            }
        }
    }

    //create this bounding box! 
    BoundingBox* thisBox = new BoundingBox(min, max);
    for(BoundingBox* b : children)
    {
        thisBox->addBox(b);
    }

    //return this bounding box 
    return thisBox;

}

World::World()
{
    ambientLight = new Color();
    *ambientLight = {80, 80, 80};;
}

int World::addObject(Object* obj)
{

    objects.push_back(obj);

    return 1;
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
        int lightBlocked = 0; 
        for(Object* obj : objects)
        {
            IntersectionInfo* lightInfo = new struct IntersectionInfo;
            float dist = obj->intersect(lightInfo, ray);
            delete lightInfo;
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
        float intersection [3] = {0,0,0};
        std::copy(std::begin(info->intersectionLocation), std::end(info->intersectionLocation), std::begin(intersection));
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

        delete color;

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

        delete specColor;

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