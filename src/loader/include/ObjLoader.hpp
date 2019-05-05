#ifndef _OBJ_LOADER_HPP_
#define _OBJ_LOADER_HPP_

#include "Loader.hpp"


//TODO : implement test 
class ObjLoader : public Loader
{
private:
    /* data */
public:
    ObjLoader();
    virtual ~ObjLoader() override;

    bool load(const std::string& path, renderer::Model &scene) override;
};



#endif // !_OBJ_LOADER_HPP_
