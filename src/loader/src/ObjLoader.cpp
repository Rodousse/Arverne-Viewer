#include "loader/ObjLoader.h"
#include <algorithm>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <plog/Log.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

ObjLoader::ObjLoader(/* args */) :
    Loader()
{

}

ObjLoader::~ObjLoader()
{

}

tinyobj::index_t findMinIndexes(const tinyobj::shape_t& shape);
tinyobj::index_t findMaxIndexes(const tinyobj::shape_t& shape);

//TODO : Insert Materials
bool ObjLoader::load(const std::string& path, std::vector<data::Mesh>& scene)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string warn;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

    std::vector<data::Mesh> meshes;

    if(!err.empty())
    {
        PLOGE << err.data() << '\n';
    }

    if(!ret)
    {
        return false;
    }

    data::Mesh newMsh;

    // Loop over shapes
    for(tinyobj::shape_t shape : shapes)
    {
        size_t index_offset = 0;

        uint32_t indexVertex = 0;

        newMsh.name = shape.name;

        data::VertexAttribute vertex;

        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];

            for(int v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                size_t indexTemp;
                data::VertexAttribute vertex;
                memset(&vertex, 0, sizeof(data::VertexAttribute));

                if(idx.vertex_index > -1)
                {
                    indexTemp = idx.vertex_index * 3;
                    vertex.pos = glm::vec3(attrib.vertices[indexTemp], -attrib.vertices[indexTemp + 2],
                                           attrib.vertices[indexTemp + 1]);
                }

                if(idx.normal_index > -1)
                {
                    indexTemp = idx.normal_index * 3;
                    vertex.normal = glm::vec3(attrib.normals[indexTemp], -attrib.normals[indexTemp + 2],
                                              attrib.normals[indexTemp + 1]);
                }

                if(idx.texcoord_index > -1)
                {
                    indexTemp = idx.texcoord_index * 2;
                    vertex.texCoord = glm::vec2(attrib.texcoords[indexTemp], attrib.texcoords[indexTemp + 1]);
                }

                //TODO : If vertex not already in vertices
                //add vertex to vertices
                //else
                //add an index to ebo

                newMsh.vertices.push_back(vertex);
                newMsh.indices.push_back(indexVertex);
                indexVertex++;
            }

            index_offset += fv;
        }

        scene.push_back(newMsh);
    }

    return true;
}



tinyobj::index_t findMinIndexes(const tinyobj::shape_t& shape)
{
    tinyobj::index_t idxTemp;
    tinyobj::index_t minIdx;

    //Look for minimum vertex index
    idxTemp = *std::min_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.vertex_index < right.vertex_index;
    });
    minIdx.vertex_index = idxTemp.vertex_index;

    //Look for minimum normal index
    idxTemp = *std::min_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.normal_index < right.normal_index;
    });
    minIdx.normal_index = idxTemp.normal_index;

    //Look for minimum texcoord_index index
    //TODO : Is there an index per mesh for tex coord
    idxTemp = *std::min_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.texcoord_index < right.texcoord_index;
    });
    minIdx.texcoord_index = idxTemp.texcoord_index;

    return minIdx;
}

tinyobj::index_t findMaxIndexes(const tinyobj::shape_t& shape)
{
    tinyobj::index_t idxTemp;
    tinyobj::index_t maxIdx;

    //Look for maximum vertex index
    idxTemp = *std::max_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.vertex_index < right.vertex_index;
    });
    maxIdx.vertex_index = idxTemp.vertex_index;

    //Look for maximum normal index
    idxTemp = *std::max_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.normal_index < right.normal_index;
    });
    maxIdx.normal_index = idxTemp.normal_index;

    //Look for maximum texcoord_index index
    //TODO : Is there an index per mesh for tex coord
    idxTemp = *std::max_element(shape.mesh.indices.begin(),
                                shape.mesh.indices.end(), [](const tinyobj::index_t& left, const tinyobj::index_t& right)
    {
        return left.texcoord_index < right.texcoord_index;
    });
    maxIdx.texcoord_index = idxTemp.texcoord_index;

    return maxIdx;
}
