#pragma once

#include "renderer/VulkanCore.h"
#include "renderer/ModelManager.h"
#include <string>
#include <memory>

namespace renderer
{

class VulkanApplication
{
protected:
    VulkanCore vkCore_;
    ModelManager modelManager_;

    virtual void createInstance();
    virtual void initWindow();

public:
    VulkanApplication();
    virtual ~VulkanApplication() = default;

    void create(VkSurfaceKHR surface);

    virtual void drawFrame();
    void resizeWindow(int width, int height);
    void cleanup();

    ModelManager& modelManager();
    void setModelManager(const ModelManager& modelManager);
    void setCamera(const Camera& camera);
    VkInstance vkInstance() const;
};

}
