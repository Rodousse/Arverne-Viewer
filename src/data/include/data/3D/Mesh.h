#pragma once

#include "data/3D/VertexAttribute.h"
#include <vector>
#include <string>

namespace data
{


struct Mesh
{
    std::string name;
    std::vector<VertexAttribute> vertices;
    std::vector<uint32_t> indices;

    Mesh() = default;
};

}
