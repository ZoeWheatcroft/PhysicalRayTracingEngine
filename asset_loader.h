#ifndef ASSET_LOADER
#define ASSET_LOADER

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "object.h"

//load a ply file and fill the vector with triangle objects
//return 1 if sucessfully loaded, 0 on fail
int loadPly(std::string filename, std::vector<Object*>* objects, float origin [3], float scale);

#endif