#ifndef COLOR_H
#define COLOR_H
#include <algorithm>
struct Color{
    float red;
    float green;
    float blue;
    void add(Color* color)
    {
        red += color->red;
        green += color->green;
        blue += color->blue;
    }
    void scale(Color* color)
    {
        red = (std::min(color->red, (float)255)/255)*red;
        green = (std::min(color->green, (float)255)/255)*green;
        blue = (std::min(color->blue, (float)255)/255)*blue;
    }
};


#endif