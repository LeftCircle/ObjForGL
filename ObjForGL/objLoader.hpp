#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>

bool is_obj_file(const std::string& path) {
	return path.substr(path.size() - 4, 4) == ".obj";
}

class ObjLoader
{
private:

public:
	ObjLoader() {};
	~ObjLoader() {};

    bool loadObjFile(const std::string& path) 
    {
        if (!is_obj_file(path)) {
            return false;
        }
        return true;
    }
};

#endif // OBJLOADER_H