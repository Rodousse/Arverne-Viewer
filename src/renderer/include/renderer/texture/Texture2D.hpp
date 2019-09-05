#pragma once

#include "renderer/VulkanCore.h"
#include <data/2D/Image.hpp>
#include <string>
#include "renderer/tools/CommandTools.h"
#include "renderer/tools/ImageTools.h"
#include "renderer/tools/MemoryTools.h"

namespace
{
template<VkFormat format>
struct MapperVkFormatToType
{
    typedef void type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R8_UNORM>
{
    typedef unsigned char type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R8G8_UNORM>
{
    typedef unsigned char type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R8G8B8_UNORM>
{
    typedef unsigned char type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R8G8B8A8_UNORM>
{
    typedef unsigned char type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R16_UNORM>
{
    typedef unsigned short type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R16G16_UNORM>
{
    typedef unsigned short type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R16G16B16_UNORM>
{
    typedef unsigned short type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R16G16B16A16_UNORM>
{
    typedef unsigned short type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R32_SFLOAT>
{
    typedef float type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R32G32_SFLOAT>
{
    typedef float type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R32G32B32_SFLOAT>
{
    typedef float type;
};

template <>
struct MapperVkFormatToType<VK_FORMAT_R32G32B32A32_SFLOAT>
{
    typedef float type;
};

} // Anonymous namespace


namespace renderer
{

template <VkFormat format>
class Texture2D
{

    using DataType = std::shared_ptr<data::Image<typename MapperVkFormatToType<format>::type>>;

public:
    Texture2D(const VulkanCore& core,
              const DataType& data):
        vkCore_(core),
        data_(data),
        format_(format)
    {
        createImage();
        createImageView();
        createSampler();
    }

    void setData(const DataType& data)
    {
        data_ = data;
    }

    const VkImage& image()const
    {
        return image_;
    }

    const VkImageView& imageView()const
    {
        return imageView_;
    }

    const VkSampler& sampler()const
    {
        return sampler_;
    }

    virtual ~Texture2D()
    {
        vkDestroyImageView(vkCore_.getDevice(), imageView_, nullptr);
        vkDestroyImage(vkCore_.getDevice(), image_, nullptr);
        vkFreeMemory(vkCore_.getDevice(), imageMemory_, nullptr);
        vkDestroySampler(vkCore_.getDevice(), sampler_, nullptr);
    }

protected:
    const VulkanCore& vkCore_;

    DataType data_;
    VkImage image_;
    VkImageView imageView_;
    VkDeviceMemory imageMemory_;
    VkFormat format_;
    VkSampler sampler_;
    uint32_t mipLevels_ = 1;

    virtual void createImage()
    {
        VkDeviceSize imageSize = data_->width() * data_->height() * 4;
        mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(data_->width(),
                                           data_->height())))) + 1;

        if(!data_->data())
        {
            throw std::runtime_error("failed to load texture image!");
        }

        mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(data_->width(),
                                           data_->height())))) + 1;

        VkBuffer stageBuffer;
        VkDeviceMemory stageBufferMemory;

        tools::memory::createBuffer(vkCore_, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stageBuffer,
                                    stageBufferMemory);
        void* pData;
        vkMapMemory(vkCore_.getDevice(), stageBufferMemory, 0, imageSize, 0, &pData);
        memcpy(pData, data_->data(), imageSize);
        vkUnmapMemory(vkCore_.getDevice(), stageBufferMemory);

        tools::image::createImage(vkCore_, static_cast<uint32_t>(data_->width()),
                                  static_cast<uint32_t>(data_->height()),
                                  mipLevels_, VK_SAMPLE_COUNT_1_BIT,
                                  tools::image::mapDataImageFormatToVkFormat(data::ImageFormat::RGBA8),
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image_, imageMemory_);
        tools::image::transitionImageLayout(vkCore_, image_,
                                            tools::image::mapDataImageFormatToVkFormat(data::ImageFormat::RGBA8),
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels_);
        //Copy the content of a VkBuffer into a VkImage format
        tools::image::copyBufferToImage(vkCore_, stageBuffer, image_,
                                        static_cast<uint32_t>(static_cast<uint32_t>(data_->width())),
                                        static_cast<uint32_t>(static_cast<uint32_t>(data_->height())));
        //Transition the layout for shader ability to read it
        //vkCore_.getUtils().transitionImageLayout(image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
        generateMipmaps(image_, tools::image::mapDataImageFormatToVkFormat(data::ImageFormat::RGBA8),
                        data_->width(),
                        data_->height(), mipLevels_);

        vkDestroyBuffer(vkCore_.getDevice(), stageBuffer, nullptr);
        vkFreeMemory(vkCore_.getDevice(), stageBufferMemory, nullptr);
    }
    virtual void createSampler()
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates =
            VK_FALSE; //Use [0,1] for coordinate not [0, texWidth/texHeight]
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if(vkCreateSampler(vkCore_.getDevice(), &samplerInfo, nullptr, &sampler_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler");
        }

    }
    virtual void createImageView()
    {
        imageView_ = tools::image::createImageView(vkCore_, format_, image_, VK_IMAGE_ASPECT_COLOR_BIT,
                     mipLevels_);
    }


    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
                         uint32_t mipLevels)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(vkCore_.getPhysicalDevice(), imageFormat, &formatProperties);

        if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("Texture Image format does not support linear blitting");
        }

        VkCommandBuffer commandBuffer = tools::command::beginSingleTimeCommands(vkCore_);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for(uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if(mipWidth > 1)
            {
                mipWidth /= 2;
            }

            if(mipHeight > 1)
            {
                mipHeight /= 2;
            }

        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        tools::command::endSingleTimeCommands(vkCore_, commandBuffer);
    }

};

}
