/*
*
*
*  Abstract class loading a 3D model File , all the meshes are stored in a vector
* of Mesh instance Vulan formated
*
*
*
*/

#pragma once

#include <iostream>
#include <string>
#include <data/3D/Mesh.h>

class Loader
{
private:


public:
    Loader();
    virtual ~Loader();
    virtual bool load(const std::string& path, std::vector<data::Mesh>& scene) = 0;
};


