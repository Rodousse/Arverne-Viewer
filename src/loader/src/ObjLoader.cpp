#include "loader/ObjLoader.h"
#include <algorithm>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
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
template <class Hasher, class Hashed>
inline void hash_combine(std::size_t& seed, const Hashed& v)
{
    Hasher hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct VertexIndicesHasher
{
    size_t operator()(const tinyobj::index_t& hashed)const
    {
        size_t seed = 0xab12f56c;
        hash_combine<std::hash<int>, int>(seed, hashed.vertex_index);
        hash_combine<std::hash<int>, int>(seed, hashed.normal_index);
        hash_combine<std::hash<int>, int>(seed, hashed.texcoord_index);
        return seed;
    }
};

struct VertexIndicesEquals
{
    bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b)const
    {
        return a.vertex_index == b.vertex_index && a.normal_index == b.normal_index
               && a.texcoord_index == b.texcoord_index;
    }
};



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
        std::unordered_map<tinyobj::index_t, uint32_t, VertexIndicesHasher, VertexIndicesEquals>
        attributeIndices;

        newMsh.name = shape.name;

        data::VertexAttribute vertex;

        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];
            //We create a normal for the face in case the normal is not present
            tinyobj::index_t& aIdx = shape.mesh.indices[index_offset];
            glm::vec3 faceNormal;

            if(aIdx.normal_index == -1)
            {
                tinyobj::index_t& bIdx = shape.mesh.indices[index_offset + 1];
                tinyobj::index_t& cIdx = shape.mesh.indices[index_offset + 2];
                glm::vec3 a = glm::vec3(attrib.vertices[aIdx.vertex_index * 3],
                                        -attrib.vertices[aIdx.vertex_index * 3 + 2],
                                        attrib.vertices[aIdx.vertex_index * 3 + 1]);
                glm::vec3 b = glm::vec3(attrib.vertices[bIdx.vertex_index * 3],
                                        -attrib.vertices[bIdx.vertex_index * 3 + 2],
                                        attrib.vertices[bIdx.vertex_index * 3 + 1]);
                glm::vec3 c = glm::vec3(attrib.vertices[cIdx.vertex_index * 3],
                                        -attrib.vertices[cIdx.vertex_index * 3 + 2],
                                        attrib.vertices[cIdx.vertex_index * 3 + 1]);
                faceNormal = glm::cross((b - a), (c - a));
            }


            for(int v = 0; v < fv; v++)
            {
                const tinyobj::index_t& idx = shape.mesh.indices[index_offset + v];

                if(attributeIndices.count(idx) == 0)
                {

                    size_t indexTemp;
                    data::VertexAttribute vertex;
                    memset(&vertex, 0, sizeof(data::VertexAttribute));

                    if(idx.vertex_index > -1)
                    {
                        indexTemp = idx.vertex_index * 3;
                        vertex.pos = glm::vec3(attrib.vertices[indexTemp], -attrib.vertices[indexTemp + 2],
                                               attrib.vertices[indexTemp + 1]);

                        if(idx.normal_index > -1)
                        {
                            indexTemp = idx.normal_index * 3;
                            vertex.normal = glm::vec3(attrib.normals[indexTemp], -attrib.normals[indexTemp + 2],
                                                      attrib.normals[indexTemp + 1]);
                        }
                        else
                        {
                            vertex.normal = faceNormal;
                        }

                        if(idx.texcoord_index > -1)
                        {
                            indexTemp = idx.texcoord_index * 2;
                            vertex.texCoord = glm::vec2(attrib.texcoords[indexTemp], attrib.texcoords[indexTemp + 1]);
                        }
                    }

                    newMsh.vertices.push_back(vertex);
                    attributeIndices[idx] = indexVertex;
                    indexVertex++;
                }

                newMsh.indices.push_back(attributeIndices[idx]);
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
