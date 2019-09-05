#pragma once

#include "renderer/VulkanCore.h"
#include "renderer/Model.h"
#include "loader/ObjLoader.h"


namespace renderer
{
class ModelManager
{
protected:
    ObjLoader loader_;

    renderer::VulkanCore* pCore_;

    std::vector<renderer::Model> models_;
    renderer::Model* selectedModel_;
    int selectedModelIndex_;

public:
    ModelManager(renderer::VulkanCore* vkCore);

    void loadNewMesh(const std::string& path);
    void setSelectedModel(int index);

    const std::vector<renderer::Model>& getModels()const;
    const renderer::Model& getSelectedModel()const;
    int getSelectedModelIndex()const;



};

}
