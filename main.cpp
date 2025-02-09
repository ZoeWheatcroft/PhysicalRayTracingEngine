#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>

#include "main.h"
#include "world.h"
#include "object.h"
#include "camera.h"

using namespace std;

const int W = 960;
const int H = 540;

#pragma pack(push, 1) // Ensure the struct is packed without padding
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

int main() {

    std::ofstream file;
    openBMP("output.bmp", W, H, &file); //open file and set header

    Camera camera;
    camera.viewpoint[0] = 0;
    camera.viewpoint[1] = 1;
    camera.viewpoint[2] = -7.8;
    camera.focalLength = 0.7;
    camera.width = 1;
    camera.height = 0.5625;

    //cast ray from viewpoint through pixels

    Sphere* sphere1 = new Sphere(0, 1.03, -4.4, 1);
    sphere1->color = {200, 200, 200};
    Sphere* sphere2 = new Sphere(0.87, 0.6, -2.0, 1);
    sphere2->color = {200, 200, 255};

    std::vector<Object*> objects;
    objects.push_back(sphere1);
    objects.push_back(sphere2);


    for(int y = 0; y < H; y++)
    {
        for(int x = 0; x < W; x++)
        {
            //project ray from viewpoint (origin) through pixel and find collision
            Color* color = new Color();
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


            if(sphere1->intersect(color, ray)){
                writePixel(((int)color->red)%255, ((int)color->green)%255, ((int)color->blue)%255, &file, x, y);
            }
            else{
                writePixel(100, 100, 255, &file, x, y);
            }
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
