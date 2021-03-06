#pragma once

#include <vulkan/vulkan.h>

namespace renderer
{

class VulkanCore;

class VkElement
{
protected:
    const VulkanCore* pCore_;
    bool isCreated_ = false;

public:
    VkElement(const VulkanCore* pCore);

    virtual void create() = 0;
    virtual void destroy() = 0;

    virtual ~VkElement();
};

}
