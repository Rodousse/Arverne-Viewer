#ifndef VULKAN_UTILS_HPP
#define VULKAN_UTILS_HPP

#include "VkElement.hpp"

namespace renderer
{

class VulkanUtils 
{
private:
	const VulkanCore *pCore_;
public:
	VulkanUtils(const VulkanCore * pCore);
	~VulkanUtils();

    VkImageView createImageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewCreateFlags flags = 0)const;
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)const;
    VkCommandBuffer beginSingleTimeCommands(bool useTransfer) const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, bool useTransfer)const;
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)const;
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)const;
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags property,
                     VkImage &image, VkDeviceMemory &imageMemory, VkImageCreateFlags flags=0)const;
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)const;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)const;
    bool hasStencilComponent(VkFormat format) const;
    VkSampleCountFlagBits getMaxUsableSampleCount();
};

}


#endif
