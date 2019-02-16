#include "Texture2D.hpp"
#include "renderer/VulkanCore.hpp"


namespace renderer
{

Texture2D::Texture2D(const VulkanCore * pCore):
    Texture2D(pCore, VK_FORMAT_R8G8B8A8_UNORM)
{
}

Texture2D::Texture2D(const VulkanCore* pCore, const VkFormat& format):
    VkElement(pCore),
    format_(format)
{
	name_ = "";
}

void Texture2D::create()
{
	createImage();
    createImageView();
    isCreated_ = true;
}

void Texture2D::destroy()
{
    if(isCreated_)
    {
        vkDestroyImageView(pCore_->getDevice(), imageView_, nullptr);
        vkDestroyImage(pCore_->getDevice(), image_, nullptr);
        vkFreeMemory(pCore_->getDevice(), imageMemory_, nullptr);
    }
}



const VkImage & Texture2D::getImage() const
{
	return image_;
}

const VkImageView & Texture2D::getImageView() const
{
	return imageView_;
}


Texture2D::~Texture2D()
{
}


}
