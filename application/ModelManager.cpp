#include "ModelManager.hpp"

ModelManager::ModelManager(renderer::VulkanCore *vkCore):
    pCore_(vkCore),
    selectedModel_(nullptr)
{

}

void ModelManager::loadNewMesh(const std::string &path)
{

    renderer::Model model(pCore_);
    loader_.load(path, model);

    std::string fileName = path.substr( path.find_last_of("/") + 1 );
    model.setName(fileName);

    models_.push_back(model);

    //setSelectedModel(models_.size()-1);
}

const std::vector<renderer::Model> &ModelManager::getModels() const
{
    return models_;
}

void ModelManager::setSelectedModel(int index)
{
    if(selectedModel_ == &models_[index])
        return;
    selectedModel_ = &models_[index];
    pCore_->setModel(*selectedModel_);
    selectedModelIndex_ = index;
}

const renderer::Model &ModelManager::getSelectedModel() const
{
    return *selectedModel_;
}

int ModelManager::getSelectedModelIndex() const
{
    return selectedModelIndex_;
}
