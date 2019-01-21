#ifndef VK_ELEMENT_HPP
#define VK_ELEMENT_HPP

#include <vulkan/vulkan.h>

class VulkanCore;


class VkElement
{
protected:
	const VulkanCore *pCore_;
    bool isCreated_ = false;

public:
	VkElement(const VulkanCore *pCore);

	virtual void create() = 0;
	virtual void destroy() = 0;

	virtual ~VkElement();
};



#endif
