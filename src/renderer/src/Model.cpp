#include "renderer/Model.h"
#include "renderer/VulkanCore.h"

namespace renderer
{

Model::Model(const VulkanCore* pCore):
    VkElement(pCore)
{
}

Model::~Model()
{
    if(isCreated_)
    {
        destroy();
    }
}

void Model::create()
{
    if(isCreated_)
    {
        destroy();
    }

    meshesData_.resize(meshes_.size());

    for(size_t idx = 0; idx < meshes_.size(); idx++)
    {
        createMeshData(meshes_[idx], meshesData_[idx]);
        //setMaterialForMesh(meshes_[idx], *defaultMaterial);
    }

    isCreated_ = true;
}

void Model::destroy()
{
    if(isCreated_)
    {
        for(auto meshData : meshesData_)
        {
            destroyMeshData(meshData);
        }

        isCreated_ = false;
    }
}

void Model::assignMesh(const std::vector<data::Mesh>& meshes)
{
    meshes_.assign(meshes.begin(), meshes.end());
}

void Model::destroyMeshData(MeshData& meshData)
{
    vkDestroyBuffer(pCore_->getDevice(), meshData.vertexBuffer, nullptr);
    vkFreeMemory(pCore_->getDevice(), meshData.vertexBufferMemory, nullptr);
    vkDestroyBuffer(pCore_->getDevice(), meshData.vertexIndexBuffer,
                    nullptr);
    vkFreeMemory(pCore_->getDevice(), meshData.vertexIndexBufferMemory,
                 nullptr);
    meshData.isAllocated = false;
}

void Model::setMaterialForMesh(const data::Mesh& mesh,
                               const Material& material)
{
    auto itFound = std::find_if(meshes_.begin(),
                                meshes_.end(), [&](data::Mesh other)
    {
        return mesh.name == other.name;
    });
    uint32_t idx = std::distance(meshes_.begin(), itFound);
    setMaterialForMeshData(meshesData_[idx], material);
}

void Model::setMaterialForMeshData(MeshData& meshData,
                                   const Material& material)
{
    meshData.material = &material;
}

void Model::createMeshData(const data::Mesh& mesh, MeshData& meshData)
{
    if(meshData.isAllocated)
    {
        destroyMeshData(meshData);
    }

    createVertexBuffer(mesh, meshData);
    createVertexIndexBuffer(mesh, meshData);
    meshData.isAllocated = true;
}

void Model::createVertexBuffer(const data::Mesh& mesh, MeshData& meshData)
{
    VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) *
                              mesh.vertices.size();
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    pCore_->getUtils().createBuffer(bufferSize, usage, properties,
                                    stagingBuffer, stagingBufferMemory);
    void* pData;//Contains a pointer to the mapped memory
    //Documentation : memory must have been created with a memory type that reports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    //                  flags is reserved for future use of the vulkanAPI.
    vkMapMemory(pCore_->getDevice(), stagingBufferMemory, 0, bufferSize, 0,
                &pData);
    memcpy(pData, meshes_[0].vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(pCore_->getDevice(), stagingBufferMemory);
    pCore_->getUtils().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, meshData.vertexBuffer,
                                    meshData.vertexBufferMemory);
    pCore_->getUtils().copyBuffer(stagingBuffer, meshData.vertexBuffer,
                                  bufferSize);
    vkDestroyBuffer(pCore_->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pCore_->getDevice(), stagingBufferMemory, nullptr);
    std::cout << "Vertex Buffer Created" << std::endl;
}

void Model::createVertexIndexBuffer(const data::Mesh& mesh,
                                    MeshData& meshData)
{
    std::cout << "Creating and Allocating Index Buffer" << std::endl;
    VkDeviceSize bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    pCore_->getUtils().createBuffer(bufferSize, usage, properties,
                                    stagingBuffer, stagingBufferMemory);
    void* pData;
    vkMapMemory(pCore_->getDevice(), stagingBufferMemory, 0, bufferSize, 0,
                &pData);
    memcpy(pData, mesh.indices.data(), (size_t)bufferSize);
    vkUnmapMemory(pCore_->getDevice(), stagingBufferMemory);
    pCore_->getUtils().createBuffer(bufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, meshData.vertexIndexBuffer,
                                    meshData.vertexIndexBufferMemory);
    pCore_->getUtils().copyBuffer(stagingBuffer, meshData.vertexIndexBuffer,
                                  bufferSize);
    vkDestroyBuffer(pCore_->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(pCore_->getDevice(), stagingBufferMemory, nullptr);
    std::cout << "Index Buffer Created" << std::endl;
}

//void Model::setDefaultMaterial(const Material &material)
//{
//    defaultMaterial = &material;
//}

const std::string& Model::getName() const
{
    return name_;
}

void Model::setName(const std::string& name)
{
    name_ = name;
}

const std::vector<MeshData>& Model::getMeshData() const
{
    return meshesData_;
}

const std::vector<data::Mesh>& Model::getMeshes() const
{
    return meshes_;
}

}
