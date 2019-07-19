#include "renderer/texture/MaterialTexture.h"
#include "renderer/VulkanCore.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace renderer
{


MaterialTexture::MaterialTexture(const renderer::VulkanCore* pCore):
    MaterialTexture(pCore, VK_FORMAT_R8G8B8A8_UNORM)
{

}


MaterialTexture::MaterialTexture(const VulkanCore* pCore, const VkFormat& format):
    MaterialTexture(pCore, "", format)
{

}

MaterialTexture::MaterialTexture(const renderer::VulkanCore* pCore, const std::string& path,
                                 const VkFormat& format):
    Texture2D(pCore, format),
    path_(path)
{

}

void MaterialTexture::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                                      int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(pCore_->getPhysicalDevice(), imageFormat, &formatProperties);

    if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("Texture Image format does not support linear blitting");
    }

    VkCommandBuffer commandBuffer = pCore_->getUtils().beginSingleTimeCommands(false);

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

    pCore_->getUtils().endSingleTimeCommands(commandBuffer, false);
}


void MaterialTexture::createSampler()
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

    if(vkCreateSampler(pCore_->getDevice(), &samplerInfo, nullptr, &sampler_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler");
    }

}


void MaterialTexture::createImage()
{

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path_.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if(!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    VkBuffer stageBuffer;
    VkDeviceMemory stageBufferMemory;

    pCore_->getUtils().createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stageBuffer,
                                    stageBufferMemory);
    void* pData;
    vkMapMemory(pCore_->getDevice(), stageBufferMemory, 0, imageSize, 0, &pData);
    memcpy(pData, pixels, imageSize);
    vkUnmapMemory(pCore_->getDevice(), stageBufferMemory);

    pCore_->getUtils().createImage(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
                                   mipLevels_, VK_SAMPLE_COUNT_1_BIT, format_, VK_IMAGE_TILING_OPTIMAL,
                                   VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image_, imageMemory_);
    pCore_->getUtils().transitionImageLayout(image_, format_, VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels_);
    //Copy the content of a VkBuffer into a VkImage format
    pCore_->getUtils().copyBufferToImage(stageBuffer, image_,
                                         static_cast<uint32_t>(static_cast<uint32_t>(texWidth)),
                                         static_cast<uint32_t>(static_cast<uint32_t>(texHeight)));
    //Transition the layout for shader ability to read it
    //pCore_->getUtils().transitionImageLayout(image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
    generateMipmaps(image_, format_, texWidth, texHeight, mipLevels_);

    vkDestroyBuffer(pCore_->getDevice(), stageBuffer, nullptr);
    vkFreeMemory(pCore_->getDevice(), stageBufferMemory, nullptr);
    stbi_image_free(pixels);
}


void MaterialTexture::createImageView()
{
    imageView_ = pCore_->getUtils().createImageView(format_, image_, VK_IMAGE_ASPECT_COLOR_BIT,
                 mipLevels_);
}


void MaterialTexture::create()
{
    Texture2D::create();
    createSampler();
}

void MaterialTexture::destroy()
{
    vkDestroySampler(pCore_->getDevice(), sampler_, nullptr);
    Texture2D::destroy();
}

void MaterialTexture::setFilePath(const std::string& path)
{
    path_.assign(path);
}

const VkSampler& MaterialTexture::getSampler() const
{
    return sampler_;
}


}
