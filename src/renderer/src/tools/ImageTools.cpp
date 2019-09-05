#include "renderer/tools/ImageTools.h"
#include "renderer/tools/MemoryTools.h"
#include "renderer/tools/CommandTools.h"

namespace renderer
{

namespace tools
{

namespace image
{

VkImageView createImageView(const VulkanCore& core, VkFormat format, VkImage image,
                            VkImageAspectFlags aspectFlags,
                            uint32_t mipLevels, VkImageViewCreateFlags flags)
{
    VkImageView imageView;

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.format = format;
    imageViewInfo.image = image;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.subresourceRange.levelCount = mipLevels;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewInfo.flags = flags;

    if(vkCreateImageView(core.getDevice(), &imageViewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view");
    }

    return imageView;
}

void createImage(const VulkanCore& core, uint32_t width, uint32_t height, uint32_t mipLevels,
                 VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags property,
                 VkImage& image, VkDeviceMemory& imageMemory, VkImageCreateFlags flags)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.format = format;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.arrayLayers = 1;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = flags;

    if(vkCreateImage(core.getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements imageMemoryRequirements = {};
    vkGetImageMemoryRequirements(core.getDevice(), image, &imageMemoryRequirements);

    VkMemoryAllocateInfo imageAllocInfo = {};
    imageAllocInfo.allocationSize = imageMemoryRequirements.size;
    imageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    imageAllocInfo.memoryTypeIndex = tools::memory::findMemoryType(core,
                                     imageMemoryRequirements.memoryTypeBits,
                                     property);

    if(vkAllocateMemory(core.getDevice(), &imageAllocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(core.getDevice(), image, imageMemory, 0);
}

void transitionImageLayout(const VulkanCore& core, VkImage image, VkFormat format,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout, uint32_t mipLevels)
{
    QueueFamilyIndices indices = core.getPhysicalDeviceProperties().getQueueFamilyIndices();
    VkCommandBuffer commandBuffer = tools::command::beginSingleTimeTransferCommands(core);

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcQueueFamilyIndex =
        VK_QUEUE_FAMILY_IGNORED; //TODO : Care here, maybe a probleme beetween transfert queue and graphics one
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if(hasStencilComponent(format))
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        if(indices.transferAvailable())
        {
            barrier.dstQueueFamilyIndex = static_cast<uint32_t>(indices.graphicsFamily);
            barrier.srcQueueFamilyIndex = static_cast<uint32_t>(indices.transferFamily);
        }
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        if(indices.transferAvailable())
        {
            barrier.dstQueueFamilyIndex = static_cast<uint32_t>(indices.graphicsFamily);
            barrier.srcQueueFamilyIndex = static_cast<uint32_t>(indices.transferFamily);
        }
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        if(indices.transferAvailable())
        {
            barrier.dstQueueFamilyIndex = static_cast<uint32_t>(indices.graphicsFamily);
            barrier.srcQueueFamilyIndex = static_cast<uint32_t>(indices.transferFamily);
        }
    }
    else
    {
        throw std::invalid_argument("Unknown layer transition");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    tools::command::endSingleTimeTransferCommands(core, commandBuffer);
}

void copyBufferToImage(const VulkanCore& core, VkBuffer buffer, VkImage image, uint32_t width,
                       uint32_t height)
{
    VkCommandBuffer commandBuffer = tools::command::beginSingleTimeTransferCommands(core);
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferImageHeight = 0;
    region.bufferRowLength = 0;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.mipLevel = 0;

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &region);

    tools::command::endSingleTimeTransferCommands(core, commandBuffer);
}

bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM_S8_UINT
           || format == VK_FORMAT_D24_UNORM_S8_UINT
           || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDeviceProperties&
        physicalDeviceProperties)
{
    VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts,
                                         physicalDeviceProperties.limits.framebufferDepthSampleCounts);

    if(counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;

    if(counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;

    if(counts &  VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;

    if(counts &  VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;

    if(counts &  VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}


VkFormat mapDataImageFormatToVkFormat(const data::ImageFormat& format)
{
    using data::ImageFormat;

    switch(format)
    {
        case ImageFormat::LUM8:
            return VK_FORMAT_R8_UNORM;

        case ImageFormat::LUM16:
            return VK_FORMAT_R16_UNORM;

        case ImageFormat::LUM32:
            return VK_FORMAT_R32_SFLOAT;

        case ImageFormat::LUMA8:
            return VK_FORMAT_R8G8_UNORM;

        case ImageFormat::LUMA16:
            return VK_FORMAT_R16G16_UNORM;

        case ImageFormat::LUMA32:
            return VK_FORMAT_R32G32_SFLOAT;

        case ImageFormat::RGB8:
            return VK_FORMAT_R8G8B8_UNORM;

        case ImageFormat::RGB16:
            return VK_FORMAT_R16G16B16_UNORM;

        case ImageFormat::RGB32:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case ImageFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case ImageFormat::RGBA16:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case ImageFormat::RGBA32:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        default:
            throw std::invalid_argument("[mapDataImageFormatToVkFormat] : Can't cast VK_FORMAT " +
                                        std::to_string(int(format)) + " to an undefined data type");
    }
}

data::ImageFormat mapVkFormatToDataImageFormat(const VkFormat& format)
{
    using data::ImageFormat;

    switch(format)
    {
        case VK_FORMAT_R8_UNORM:
            return ImageFormat::LUM8;

        case VK_FORMAT_R16_UNORM:
            return ImageFormat::LUM16;

        case VK_FORMAT_R32_SFLOAT:
            return ImageFormat::LUM32;

        case VK_FORMAT_R8G8_UNORM:
            return ImageFormat::LUMA8;

        case VK_FORMAT_R16G16_UNORM:
            return ImageFormat::LUMA16;

        case VK_FORMAT_R32G32_SFLOAT:
            return ImageFormat::LUMA32;

        case VK_FORMAT_R8G8B8_UNORM:
            return ImageFormat::RGB8;

        case VK_FORMAT_R16G16B16_UNORM:
            return ImageFormat::RGB16;

        case VK_FORMAT_R32G32B32_SFLOAT:
            return ImageFormat::RGB32;

        case VK_FORMAT_R8G8B8A8_UNORM:
            return ImageFormat::RGBA8;

        case VK_FORMAT_R16G16B16A16_UNORM:
            return ImageFormat::RGBA16;

        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return ImageFormat::RGBA32;

        default:
            throw std::invalid_argument("[mapVkFormatToDataImageFormat] : Can't cast VK_FORMAT " +
                                        std::to_string(format) + " to an undefined data type");

    }
}

}

}

}
