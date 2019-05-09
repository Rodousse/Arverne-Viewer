#pragma once

#include "loader/Loader.h"

class ObjLoader : public Loader
{
private:
    /* data */
public:
    ObjLoader();
    virtual ~ObjLoader() override;

    bool load(const std::string& path, std::vector<data::Mesh>& scene) override;
};
