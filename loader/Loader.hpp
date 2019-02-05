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
