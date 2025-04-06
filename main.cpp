#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>

#include "main.h"
#include "world.h"
#include "object.h"
#include "camera.h"
#include "math.h"

using namespace std;

const int W = 960;
const int H = 540;

World* world;
int MAX_DEPTH = 10;

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

void illuminate(Color* color, Ray ray, int depth){

    float smallestDist = -1;
    IntersectionInfo* closestIntersection;
    Object* closestObj;
    for(Object* obj : world->objects)
    {
        IntersectionInfo* info = new struct IntersectionInfo;
        float dist = obj->intersect(info, ray);
        if(dist > 0.0001 && (smallestDist == -1 || dist < smallestDist)){
            smallestDist = dist;
            closestIntersection = info;
            closestObj = obj;
        }
    }

    if(smallestDist == -1)
    {
        *color = {100, 100, 250};
    }
    else{
        //we should now theoretically have intersection
        //update intersection info with texture color
        closestObj->getTextureColor(closestIntersection);

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
                luminance->add(reflectionColor);
            }
        }
        //tone reproduction
        *color = closestIntersection->mat.color;
        color->scale(luminance);

    }
}


//255,255,255 is white and 0,0,0 is black
int main() {

    std::ofstream file;
    openBMP("images/output.bmp", W, H, &file); //open file and set header

    Camera camera;
    camera.viewpoint[0] = 1.5;
    camera.viewpoint[1] = 1;
    camera.viewpoint[2] = -6.8;
    camera.focalLength = 0.5;
    camera.width = 1;
    camera.height = 0.5625;

    //cast ray from viewpoint through pixels

    Sphere* sphere1 = new Sphere(0, 1.43, -3.3, 1);
    sphere1->mat.color = {0, 200, 255};
    //sphere1->mat.kR = 1.0;
    Sphere* sphere2 = new Sphere(2.2, 1.1, -2.0, 1);
    sphere2->mat.color = {200, 200, 255};
    sphere2->mat.kR = 1.0;

    Light* light = new Light(3, 20, -3.0, 0.5);
    light->mat.color = {255, 255, 255};

    Light* light_blue = new Light(-3, 20, 3.0, 0.5);
    light_blue->mat.color = {0, 0, 255};
    
    Triangle* triangle = new Triangle();
    triangle->point0[X_AXIS] = -1.0; triangle->point0[Y_AXIS] = 0; triangle->point0[Z_AXIS] = -8;
    triangle->point1[X_AXIS] = 2.6; triangle->point1[Y_AXIS] = 0; triangle->point1[Z_AXIS] = 0;
    triangle->point2[X_AXIS] = -1.0; triangle->point2[Y_AXIS] = 0; triangle->point2[Z_AXIS] = 0;

    triangle->mat.color = {255, 100, 100};
    triangle->texture = TextureEnum::CHECKER;

    Triangle* triangle2 = new Triangle();
    triangle2->point0[X_AXIS] = 2.6; triangle2->point0[Y_AXIS] = 0; triangle2->point0[Z_AXIS] = -8;
    triangle2->point1[X_AXIS] = 2.6; triangle2->point1[Y_AXIS] = 0; triangle2->point1[Z_AXIS] = 0;
    triangle2->point2[X_AXIS] = -1.0; triangle2->point2[Y_AXIS] = 0; triangle2->point2[Z_AXIS] = -8;
    triangle2->mat.color = {255, 100, 100};
    triangle2->texture = TextureEnum::CHECKER;

    //fun test for sea urchin wizard 
    Triangle* spike = new Triangle();
    spike->point0[X_AXIS] = 1,57; spike->point0[Y_AXIS] = 2; spike->point0[Z_AXIS] = -3;
    spike->point1[X_AXIS] = -1.0; spike->point1[Y_AXIS] = 2; spike->point1[Z_AXIS] = -3;
    spike->point2[X_AXIS] = 0; spike->point2[Y_AXIS] = 4; spike->point2[Z_AXIS] = -3;
    spike->mat.color = {255, 255, 0};

    //add everything into the world
    world = new World();
    world->addObject(sphere1);
    world->addObject(sphere2);
    world->addObject(triangle2);
    world->addObject(triangle);

    world->addLight(light);
    //world->addLight(light_blue);

    world->camera = &camera;

    world->objects[0]->mat;

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

            writePixel(((int)color->red)%256, ((int)color->green)%256, ((int)color->blue)%256, &file, x, y);
        }
    }

    file.close();

    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout 
        << "\n"
        << (now->tm_year + 1900) << '-' 
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday
         <<" minute: "
         << now->tm_min
         << "\n";

    return 0;
}
