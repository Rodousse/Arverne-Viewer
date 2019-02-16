/*
 *
 *
 *  Abstract class loading a 3D model File , all the meshes are stored in a vector
 * of Mesh instance Vulan formated
 *
 *
 *
 */



#ifndef _LOADER_HPP_
#define _LOADER_HPP_

#include <iostream>
#include <string>
#include <renderer/Mesh.hpp>



class Loader
{
private:
    

public:
    Loader();
    virtual ~Loader();
    virtual bool load(const std::string& path, std::vector<renderer::Mesh>& scene)=0;
};



#endif // !_LOADER_HPP_
