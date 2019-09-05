#include "renderer/ModelManager.h"
namespace renderer
{
ModelManager::ModelManager(renderer::VulkanCore* vkCore):
    pCore_(vkCore),
    selectedModel_(nullptr)
{

}

void ModelManager::loadNewMesh(const std::string& path)
{
    renderer::Model model(pCore_);

    std::vector<data::Mesh> scene;
    loader_.load(path, scene);
    model.assignMesh(scene);

    std::string fileName = path.substr(path.find_last_of("/") + 1);
    model.setName(fileName);

    models_.push_back(model);
}

const std::vector<renderer::Model>& ModelManager::getModels() const
{
    return models_;
}

void ModelManager::setSelectedModel(int index)
{
    if(selectedModel_ == &models_[index])
    {
        return;
    }

    selectedModel_ = &models_[index];
    pCore_->setModel(*selectedModel_);
    selectedModelIndex_ = index;
}

const renderer::Model& ModelManager::getSelectedModel() const
{
    return *selectedModel_;
}

int ModelManager::getSelectedModelIndex() const
{
    return selectedModelIndex_;
}

}
