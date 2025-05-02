#ifndef COLOR_H
#define COLOR_H
#include <algorithm>
struct Color{
    float red = 0;
    float green = 0;
    float blue = 0;
    void add(Color* color)
    {
        red += color->red;
        green += color->green;
        blue += color->blue;
    }
    void scale(Color* color)
    {
        float redScale = color->red/255;
        float gScale = color->green/255;
        float bScale = color->blue/255;
        red = red * redScale;
        green = green * gScale; 
        blue = blue * bScale;
        // red = (std::min(color->red, (float)255)/255)*red;
        // green = (std::min(color->green, (float)255)/255)*green;
        // blue = (std::min(color->blue, (float)255)/255)*blue;
    }
    void scale(float f)
    {
        red = red * f;
        green = green * f;
        blue = blue * f;
    }
};


#endif