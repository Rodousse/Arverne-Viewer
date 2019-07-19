#pragma once

#include "renderer/VkElement.h"
#include <string>

namespace renderer
{

class Texture2D :
    public VkElement
{
protected:
    using VkElement::pCore_;
    std::string name_;

    VkImage image_;
    VkImageView imageView_;
    VkDeviceMemory imageMemory_;
    VkFormat format_;

    virtual void createImage() = 0;
    virtual void createImageView() = 0;

public:
    Texture2D(const VulkanCore* pCore);
    Texture2D(const VulkanCore* pCore, const VkFormat& format);
    virtual void create() override;
    virtual void destroy() override;

    const VkImage& getImage()const;
    const VkImageView& getImageView()const;

    ~Texture2D() override;
};

}
