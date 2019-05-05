#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "renderer/VulkanCore.hpp"
#include "renderer/Model.hpp"
#include "loader/ObjLoader.hpp"

class ModelManager
{
protected:

    ObjLoader loader_;

    renderer::VulkanCore *pCore_;

    std::vector<renderer::Model> models_;
    renderer::Model *selectedModel_;
    int selectedModelIndex_;



public:
    ModelManager(renderer::VulkanCore *vkCore);

    void loadNewMesh(const std::string& path);


    const std::vector<renderer::Model>& getModels()const;
    const renderer::Model& getSelectedModel()const;
    int getSelectedModelIndex()const;

    void setSelectedModel(int index);



};

#endif // MODELMANAGER_H
