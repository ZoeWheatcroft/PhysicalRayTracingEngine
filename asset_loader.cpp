#include "asset_loader.h"

struct Vertex{
    float x;
    float y;
    float z;

    Vertex(): x(0), y(0), z(0) {}

    Vertex(float xv, float yv, float zv) : x(xv), y(yv), z(zv) {}
};

Object* makeTriangle(Vertex* p0, Vertex* p1, Vertex* p2){

    Triangle* tri = new Triangle();

    tri->point0[X_AXIS] = p0->x; tri->point0[Y_AXIS] = p0->y; tri->point0[Z_AXIS] = p0->z;
    tri->point1[X_AXIS] = p1->x; tri->point1[Y_AXIS] = p1->y; tri->point1[Z_AXIS] = p1->z;
    tri->point2[X_AXIS] = p2->x; tri->point2[Y_AXIS] = p2->y; tri->point2[Z_AXIS] = p2->z;

    //tri->mat.color = {100+abs(p0->x*100), 100+abs(p0->y*100), 100+abs(p0->z*100)};
    tri->mat.color = {200, 200, 200};
    tri->mat.kR = 0.0;
    tri->mat.kE = 50.0;

    return tri;
}

int loadPly(std::string filename, std::vector<Object *>* objects, float origin [3], float scale)
{
    std::ifstream plyFile;
    std::string line;
    
    plyFile.open(filename);

    plyFile.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);

    try{
        // check that this is a ply file 
        std::getline(plyFile, line);
        if(line != "ply"){
            printf("\n Could not load %s, incorrect file format", filename);
            printf("\nFailed on: %s", line.c_str());
            return 0;
        }
        //check format
        std::getline(plyFile, line);
        if(line.find("ascii") == std::string::npos){
            printf("\n PLY was not ASCII, could not load");
            printf("\nFailed on: %s", line.c_str());
            return 0;
        }
        //check for comments
        while(true){
            std::getline(plyFile, line);
            if(line.find("comment") == std::string::npos){
                break;
            }
        }

        //check for number of vertexes-- this will be used to create a native array of that size
        const int numVertex = std::stoi(line.substr(15));

        //skip properties 
        while(true){
            std::getline(plyFile, line);
            if(line.find("property") == std::string::npos){
                break;
            }
        }

        //check for number of faces-- this will be how many objects to create
        int numFace = std::stoi(line.substr(13));
        
        // property list? skip
        while(true){
            std::getline(plyFile, line);
            if(line.find("property") == std::string::npos){
                break;
            }
        }

        //once get end header, skip to reading in vertices 
        // this is nice as well because it does not read a line in
        // thus, the vertex and face read ins call getline at top of for loop w/o issue
        if(line.find("end_header") == std::string::npos){
            printf("\nFailed on: %s", line.c_str());
            printf("\n could not find header end");
            return 0;
        }
        
        //read in verticies 
        std::vector<Vertex*> verticies;
        for(int i = 0; i < numVertex; i++)
        {
            std::getline(plyFile, line);

            const char* str = line.c_str();
            char* end;

            float x = std::strtof(str, &end)*scale + origin[0];
            float y = std::strtof(end, &end)*scale + origin[1];
            float z = std::strtof(end, &end)*scale + origin[2];

            verticies.push_back(new Vertex(x, y, z));
        }

        //read in faces-- make triangles 
        for(int i = 0; i < numFace; i++)
        {
            std::getline(plyFile, line);

            const char* str = line.c_str();
            char* end;

            //the number of vertices in the face
            //TODO
            /**
             * theoretically, if we wanted to read in non-triangles, special code would be needed here 
             * to create the correct kind of polygon-- not just a triangle 
             */
            int faceVertexCount = std::strtof(str, &end);
            int p0 = std::strtol(end, &end, 10);
            int p1 = std::strtol(end, &end, 10);
            int p2 = std::strtol(end, &end, 10);

            Object* obj = makeTriangle(verticies[p0], verticies[p1], verticies[p2]);

            objects->push_back(obj);
        }

        return 1;
    }
    catch(const std::ifstream::failure& e){
        printf("\nCrashed out");
        return 0;
    }

    return 0;
}