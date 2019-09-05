#pragma once

#include "renderer/VulkanCore.h"
#include "data/2D/Image.hpp"

namespace renderer
{

namespace tools
{

namespace image
{

VkImageView createImageView(const VulkanCore& core, VkFormat format, VkImage image,
                            VkImageAspectFlags aspectFlags,
                            uint32_t mipLevels, VkImageViewCreateFlags flags = 0);

void createImage(const VulkanCore& core, uint32_t width, uint32_t height, uint32_t mipLevels,
                 VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags property,
                 VkImage& image, VkDeviceMemory& imageMemory, VkImageCreateFlags flags = 0);

void transitionImageLayout(const VulkanCore& core, VkImage image, VkFormat format,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout, uint32_t mipLevels);

void copyBufferToImage(const VulkanCore& core, VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height);

bool hasStencilComponent(VkFormat format) ;

VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDeviceProperties&
        physicalDeviceProperties);


VkFormat mapDataImageFormatToVkFormat(const data::ImageFormat& format);

data::ImageFormat mapVkFormatToDataImageFormat(const VkFormat& format);

}

}

}
