#ifndef MODEL_H
#define MODEL_H

#include "Mesh.hpp"
#include "VkElement.hpp"
#include "Material.hpp"

namespace renderer{

struct MeshData{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer vertexIndexBuffer;
    VkDeviceMemory vertexIndexBufferMemory;
    bool isAllocated = false;

    Material const* material;
};

class Model : public VkElement
{

protected:
    using VkElement::pCore_;

    std::string name_;

    std::vector<Mesh> meshes_;
    std::vector<MeshData> meshesData_;
    //static Material const* defaultMaterial;

    //Allocate the memory on device for the specified mesh in the meshdata
    void createMeshData(const Mesh& mesh, MeshData& meshData);
    void createVertexBuffer(const Mesh& mesh, MeshData& meshData);
    void createVertexIndexBuffer(const Mesh& mesh, MeshData& meshData);
    void destroyMeshData(MeshData& meshData);


    void setMaterialForMeshData(MeshData& meshData, const Material& material);


public:
    Model(const VulkanCore *pCore);

    virtual void create() override;
    virtual void destroy() override;

    void assignMesh(const std::vector<Mesh>& meshes);
    void clearMesh();

    void setMaterialForMesh(const Mesh& mesh, const Material& material);


    //static void setDefaultMaterial(const Material& material);


    const std::string& getName() const;
    void setName(const std::string &name);
    const std::vector<MeshData>& getMeshData()const;
    const std::vector<Mesh>& getMeshes()const;


    virtual ~Model() override;
};


}

#endif // MODEL_H
