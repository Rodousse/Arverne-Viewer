#ifndef MESH_HPP
#define MESH_HPP

#include "Vertex.hpp"
#include <vector>


namespace renderer
{

struct Mesh
{
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh();
};

}

#endif // MESH_HPP
