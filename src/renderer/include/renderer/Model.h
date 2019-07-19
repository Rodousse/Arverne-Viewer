#pragma once

#include "data/3D/Mesh.h"
#include "renderer/VkElement.h"
#include "renderer/Material.h"

namespace renderer
{

struct MeshData
{
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

    std::vector<data::Mesh> meshes_;
    std::vector<MeshData> meshesData_;
    //static Material const* defaultMaterial;

    //Allocate the memory on device for the specified mesh in the meshdata
    void createMeshData(const data::Mesh& mesh, MeshData& meshData);
    void createVertexBuffer(const data::Mesh& mesh, MeshData& meshData);
    void createVertexIndexBuffer(const data::Mesh& mesh, MeshData& meshData);
    void destroyMeshData(MeshData& meshData);


    void setMaterialForMeshData(MeshData& meshData,
                                const Material& material);

public:
    Model(const VulkanCore* pCore);

    virtual void create() override;
    virtual void destroy() override;

    void assignMesh(const std::vector<data::Mesh>& meshes);
    void clearMesh();

    void setMaterialForMesh(const data::Mesh& mesh, const Material& material);
    //static void setDefaultMaterial(const Material& material);

    const std::string& getName() const;
    void setName(const std::string& name);
    const std::vector<MeshData>& getMeshData()const;
    const std::vector<data::Mesh>& getMeshes()const;

    virtual ~Model() override;
};


}
