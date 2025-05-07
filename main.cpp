#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
#include <queue>
#include <chrono>

#include "main.h"
#include "world.h"
#include "object.h"
#include "camera.h"
#include "math.h"
#include "asset_loader.h"

using namespace std;

const int W = 960;
const int H = 540;

World* world;
int MAX_DEPTH = 4;

int SUPER_SAMPLING = 0;

int USE_BOUNDING = 1;

#pragma pack(push, 1) // Ensure the struct is packed without padding0
struct BMPHeader {
    uint16_t fileType{0x4D42};   // "BM"
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54}; // Header size

    uint32_t size{40};       // DIB header size
    int32_t width{0};        // Image width
    int32_t height{0};       // Image height
    uint16_t planes{1};      // num color planes (must be 1)
    uint16_t bitCount{24};   // 24-bit color
    uint32_t compression{0}; // compression-- none rn but png later? 
    uint32_t sizeImage{0};   // image size-- using BI_RGB for compression so size can be 0
    int32_t xPixelsPerMeter{0}; 
    int32_t yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)


void openBMP(const std::string& filename, int width, int height, std::ofstream* file) {
    BMPHeader header;
    header.width = width;
    header.height = -height;  // Negative to store pixels top-down
    int rowPadding = (4 - (W * 3) % 4) % 4;
    header.sizeImage = (width * 3 + rowPadding) * height;
    header.fileSize = header.offsetData + header.sizeImage;

    file->open(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing.\n";
        exit;
    }

    file->write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    std::cout << "BMP file opened successfully: " << filename << std::endl;

}

void writePixel(int red, int green, int blue, std::ofstream* file, int x, int y)
{
    // because we're using bitmap, we write bgr
    file->write(reinterpret_cast<const char*>(&blue), 1);
    file->write(reinterpret_cast<const char*>(&green), 1);
    file->write(reinterpret_cast<const char*>(&red), 1);

    //we have padding at end of each row 
    if (x % W + 1 == 1)
    {
        char padding[3] = {0, 0, 0};
        file->write(padding, (4 - (W * 3) % 4) % 4); // Each row must be a multiple of 4 bytes);
    }
}

int getIntersectionsInAABB(Ray* ray, vector<Object*>* intersectingObjects){
    // get child boxes that we intersect with 
    // get children of children boxes that we are intersecing with 
    // if not intersecting, return 
    queue<BoundingBox*> queue;
    queue.push(world->rootBox);

    int s = 0;

    while(!queue.empty()){
        //get current box from front of queue 
        BoundingBox* box = queue.front();
        queue.pop();
        //check if box intersects with ray
        if(box->intersect(ray))
        {
            // if it contains no boxes, it just contains an object-- add object and return 
            if(box->childBoxes.size() == 0){
                intersectingObjects->push_back(box->childObject);
                s += 1;
            }
            else{
                // add all child boxes to queue 
                for(BoundingBox* b : box->childBoxes){
                    queue.push(b);
                }
            }
        }
    }
    return s;
}

void illuminate(Color* color, Ray ray, int depth){

    float smallestDist = -1;
    IntersectionInfo* closestIntersection;

    //objects in range of ray
    //either all objects in world, or objects within ray-intersecting boxes
    vector<Object*> objectsInRange;
    if(USE_BOUNDING)
    {
        int s = getIntersectionsInAABB(&ray, &objectsInRange);
    }
    else{
        objectsInRange = world->objects;
    }

    for(Object* obj : objectsInRange)
    {
        IntersectionInfo* info = new struct IntersectionInfo;
        float dist = obj->intersect(info, ray);
        if(dist > 0.0001 && (smallestDist == -1 || dist < smallestDist)){
            smallestDist = dist;
            delete closestIntersection;
            closestIntersection = info;
            obj->getTextureColor(closestIntersection);
        }
        else{
            delete info;
        }
    }

    if(smallestDist == -1)
    {
        *color = {100, 100, 250};
    }
    else{
        //we should now theoretically have intersection
        //update intersection info with texture color

        //use intersection info with light to get light values
        Color* luminance = new struct Color;
        world->applyPhong(closestIntersection, luminance);
        
        // if it's reflective, does that mean it's getting the color of what it's reflecting or the light or??? 

        if(depth < MAX_DEPTH)
        {
            //if thing intersected with is reflective 
            if(closestIntersection->mat.kR > 0)
            {
                //get the reflection vector
                Ray* reflectionRay = new Ray();
                world->getReflectionVector(closestIntersection, ray, reflectionRay);
                Color* reflectionColor = new Color();
                illuminate(reflectionColor, *reflectionRay, depth + 1);
                reflectionColor->scale(closestIntersection->mat.kR);
                luminance->add(reflectionColor);
                
                delete reflectionColor;
                delete reflectionRay;
            }
            if(closestIntersection->mat.kT > 0)
            {
                //get the transmission vector
                Ray* transmissionRay = new Ray();
                world->getTransmissionVector(closestIntersection, ray, transmissionRay);
                Color* transmissionColor = new Color();
                illuminate(transmissionColor, *transmissionRay, depth + 1);
                transmissionColor->scale(closestIntersection->mat.kT);
                luminance->add(transmissionColor);
                
                delete transmissionColor;
                delete transmissionRay;
            }
        }
        //tone reproduction
        *color = closestIntersection->mat.color;
        color->scale(luminance);

        delete luminance;
    }

    delete closestIntersection;
}

void makeWhittedObjects(){
    Sphere* sphere1 = new Sphere(0, 0.9, -4.0, 0.8);
    sphere1->mat.color = {50, 50, 255};
    sphere1->mat.kR = 0;
    sphere1->mat.kE = 200.0;
    sphere1->mat.kS = 0.5;
    sphere1->mat.kD = 0.5;
    sphere1->mat.kT = 0;

    Sphere* sphere2 = new Sphere(1.27, 0.7, -3.3, 0.6);
    sphere2->mat.color = {255, 255, 255};
    sphere2->mat.kR = 1.0;

    Light* light_blue = new Light(-3, 20, 3.0, 0.5);
    light_blue->mat.color = {0, 0, 255};
    
    Triangle* triangle = new Triangle();
    // Original: point0 → point1 → point2 (CW)
    // New: point0 → point2 → point1 (CCW)
    triangle->point0[X_AXIS] = -1.8; triangle->point0[Y_AXIS] = 0; triangle->point0[Z_AXIS] = -8;
    triangle->point1[X_AXIS] = -1.8; triangle->point1[Y_AXIS] = 0; triangle->point1[Z_AXIS] = 0;
    triangle->point2[X_AXIS] =  2.6; triangle->point2[Y_AXIS] = 0; triangle->point2[Z_AXIS] = 0;
    triangle->mat.color = {255, 100, 100};
    triangle->texture = TextureEnum::CHECKER;
    
    Triangle* triangle2 = new Triangle();
    // Original: point0 → point1 → point2 (CW)
    // New: point0 → point2 → point1 (CCW)
    triangle2->point0[X_AXIS] =  2.6; triangle2->point0[Y_AXIS] = 0; triangle2->point0[Z_AXIS] = -8;
    triangle2->point1[X_AXIS] = -1.8; triangle2->point1[Y_AXIS] = 0; triangle2->point1[Z_AXIS] = -8;
    triangle2->point2[X_AXIS] =  2.6; triangle2->point2[Y_AXIS] = 0; triangle2->point2[Z_AXIS] =  0;
    triangle2->mat.color = {255, 100, 100};
    triangle2->texture = TextureEnum::CHECKER;

    //add everything into the world
    world->addObject(sphere1);
    world->addObject(sphere2);
    world->addObject(triangle2);
    world->addObject(triangle);
    //world->addLight(light_blue);
}

void makeTestSpheres(){
    //make test spheres
    Sphere* sphere3 = new Sphere(-0.03, 0.12, 0.004, 0.3);
    sphere3->mat.color = {255, 100, 0};
    Sphere* sphere4 = new Sphere(-1.5, 1, -3, 0.3);
    sphere4->mat.color = {0, 100, 255};
    Sphere* sphere5 = new Sphere(-2, 1, -3, 0.3);
    sphere5->mat.color = {0, 100, 255};

    //add test spheres 
    world->addObject(sphere3);
    world->addObject(sphere4);
    world->addObject(sphere5);
}

void bunnyTestTriangle(){
    // bunny test triangle
    Triangle* tri = new Triangle();

    // Line 1: -0.0378297 0.12794 0.00447467 0.850855 0.5
    tri->point0[X_AXIS] = -0.0378297f;
    tri->point0[Y_AXIS] =  0.12794f;
    tri->point0[Z_AXIS] =  0.00447467f;
    
    // Line 2: -0.0447794 0.128887 0.00190497 0.900159 0.5
    tri->point1[X_AXIS] = -0.0447794f;
    tri->point1[Y_AXIS] =  0.128887f;
    tri->point1[Z_AXIS] =  0.00190497f;
    
    // Line 3: -0.0680095 0.151244 0.0371953 0.398443 0.5
    tri->point2[X_AXIS] = -0.0680095f;
    tri->point2[Y_AXIS] =  0.151244f;
    tri->point2[Z_AXIS] =  0.0371953f;
    
    // Set color (yellow)
    tri->mat.color = {255, 255, 0};
    
    // Add to world
    world->addObject(tri);
}

//255,255,255 is white and 0,0,0 is black
int main() {

    std::ofstream file;
    openBMP("images/output.bmp", W, H, &file); //open file and set header
    
    world = new World();

    Camera camera;
    camera.viewpoint[0] = 0;
    camera.viewpoint[1] = 1;
    camera.viewpoint[2] = -6.8;
    camera.focalLength = 0.5;
    camera.width = 1;
    camera.height = 0.5625;

    Light* light = new Light(10, 10, -5.0, 0.5);
    light->mat.color = {255, 255, 255};
    world->addLight(light);

    Light* light2 = new Light(-10, 10, -5.0, 0.5);
    light2->mat.color = {255, 0, 100};
    //world->addLight(light2);

    //make and add objects 
    makeWhittedObjects();

    //stanford bunny! 
    float origin [3] = {0,-0.4,-3.75};
    float scale = 8;
    //todo 
    //delete vectors
    //loadPly("assets/stanford_bunny.ply", &world->objects, origin, scale);
    //loadPly("assets/stanford_bunny_1k.ply", &world->objects, origin, scale);
    //loadPly("assets/stanford_bunny_4k.ply", &world->objects, origin, scale);
    //loadPly("assets/stanford_bunny_16k.ply", &world->objects, origin, scale);
    //loadPly("assets/pyramid.ply", &world->objects, origin, scale);

    printf("\nNumber of objects in world: %d", world->objects.size());

    //fun test for sea urchin wizard 
    Triangle* spike = new Triangle();
    spike->point0[X_AXIS] = 1,57; spike->point0[Y_AXIS] = 2; spike->point0[Z_AXIS] = -3;
    spike->point1[X_AXIS] = -2.0; spike->point1[Y_AXIS] = 2; spike->point1[Z_AXIS] = -3;
    spike->point2[X_AXIS] = 0; spike->point2[Y_AXIS] = 4; spike->point2[Z_AXIS] = -3;
    spike->mat.color = {255, 255, 0};
    //world->addObject(spike);

    //makeTestSpheres();

    world->camera = &camera;

    // if using bounding for intersection detection, construct axis-aligned bounding boxes (AABB)
    if(USE_BOUNDING){
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        world->boxAllObjects();
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        printf("\nBuilding kd tree took %d microseconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end-start));
    }

    std::chrono::time_point<std::chrono::system_clock> renderingStart = std::chrono::system_clock::now();
    for(int y = 0; y < H; y++)
    {
        for(int x = 0; x < W; x++)
        {
            //project ray from viewpoint (origin) through pixel and find collision
            Color* color = new Color();
            *color = {10, 255, 10};
            Ray ray;
            std::copy(std::begin(camera.viewpoint), std::end(camera.viewpoint), std::begin(ray.origin));
            //calculate normalized direction
            float pixelWidth = camera.width/W; 
            float pixelHeight = camera.height/H;   

            if(!SUPER_SAMPLING)
            {
                //pixel location = center of camera - width/2 (far left) + pixelWidth*x + pixelWidth/2(to center in middle of pixel)
                float pixelX = camera.viewpoint[X_AXIS] - camera.width/2 + pixelWidth*x + pixelWidth/2;
                float pixelY = camera.viewpoint[Y_AXIS] + camera.height/2 - pixelHeight*y - pixelHeight/2;
                float dir [3] = {pixelX - ray.origin[X_AXIS], pixelY - ray.origin[Y_AXIS], camera.viewpoint[Z_AXIS]  + camera.focalLength - camera.viewpoint[Z_AXIS]};
    
                    //normalize the direction
                float magnitude = sqrt(pow(dir[X_AXIS], 2) + pow(dir[Y_AXIS], 2) + pow(dir[Z_AXIS], 2));
                for(int i = 0; i < 3; i++)
                {
                    dir[i] = dir[i]/magnitude;
                }
                std::copy(std::begin(dir), std::end(dir), std::begin(ray.direction));
    
                illuminate(color, ray, 0);
            }
            else
            {
                Color* accumulateColor = new Color(); //add all the colors of the four quarters of the pixel, will be divided later
                // do 4 rays by dividing pixel into four quarters
                for(int s = 0; s < 4; s++)
                {
                    *color = {0,0,0}; 
                    //pixel location = center of camera - width/2 (far left) + pixelWidth*x + pixelWidth/2(to center in middle of pixel)
                    float pixelX = camera.viewpoint[X_AXIS] - camera.width/2 + pixelWidth*x + (pixelWidth/4)*(s%2); 
                    float pixelY = camera.viewpoint[Y_AXIS] + camera.height/2 - pixelHeight*y - pixelHeight/2*((s/2)%2);
                    float dir [3] = {pixelX - ray.origin[X_AXIS], pixelY - ray.origin[Y_AXIS], camera.viewpoint[Z_AXIS]  + camera.focalLength - camera.viewpoint[Z_AXIS]};
        
                        //normalize the direction
                    float magnitude = sqrt(pow(dir[X_AXIS], 2) + pow(dir[Y_AXIS], 2) + pow(dir[Z_AXIS], 2));
                    for(int i = 0; i < 3; i++)
                    {
                        dir[i] = dir[i]/magnitude;
                    }
                    std::copy(std::begin(dir), std::end(dir), std::begin(ray.direction));
        
                    illuminate(color, ray, 0);
                    accumulateColor->add(color);
                }
                accumulateColor->scale(0.25);
                *color = *accumulateColor;
                delete accumulateColor;
            }

            writePixel(std::min(((int)color->red), 255), std::min((int)color->green, 255), std::min((int)color->blue, 255), &file, x, y);
            delete color;
        }
    }

    file.close();

    
    std::chrono::time_point<std::chrono::system_clock> renderingEnd = std::chrono::system_clock::now();
    printf("\nRendering took %d seconds\n", std::chrono::duration_cast<std::chrono::seconds>(renderingEnd-renderingStart));


    return 0;
}
