#ifndef CAMERA_H
#define CAMERA_H


class Camera{

    public:
        float viewpoint [3] = {0,0,0}; 
        float focalLength; 
        float width;
        float height;

    void test();
};


#endif