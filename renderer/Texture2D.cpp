#include "Texture2D.hpp"
#include "VulkanCore.hpp"


namespace renderer
{

void Texture2D::createImage()
{

    QImage img(QString::fromStdString(path_));
    if(img.isNull()){
        qFatal("failed to load the image");
    }

    img = img.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(img.width(), img.height())))) + 1;
    VkBuffer stageBuffer;
    VkDeviceMemory stageBufferMemory;
    VkDeviceSize texSize = static_cast<unsigned long long>(img.width() * img.height() * 4);

    pCore_->getUtils().createBuffer(texSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stageBuffer, stageBufferMemory);
    void *pData;
    vkMapMemory(pCore_->getDevice(), stageBufferMemory, 0, texSize, 0, &pData);
    memcpy(pData, img.bits(), texSize);
    vkUnmapMemory(pCore_->getDevice(), stageBufferMemory);

    //---------------------------------------------------------------

    pCore_->getUtils().createImage(static_cast<uint32_t>(img.width()), static_cast<uint32_t>(img.height()), mipLevels_, VK_SAMPLE_COUNT_1_BIT, format_, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image_, imageMemory_);
    pCore_->getUtils().transitionImageLayout(image_, format_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels_);
	//Copy the content of a VkBuffer into a VkImage format
	pCore_->getUtils().copyBufferToImage(stageBuffer, image_,
        static_cast<uint32_t>(static_cast<uint32_t>(img.width())),
        static_cast<uint32_t>(static_cast<uint32_t>(img.height())));
	//Transition the layout for shader ability to read it
    //pCore_->getUtils().transitionImageLayout(image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
    generateMipmaps(image_, format_, img.width(), img.height(), mipLevels_);

	vkDestroyBuffer(pCore_->getDevice(), stageBuffer, nullptr);
	vkFreeMemory(pCore_->getDevice(), stageBufferMemory, nullptr);
}

void Texture2D::createImageView()
{
    imageView_ = pCore_->getUtils().createImageView(format_, image_, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels_);
}

void Texture2D::createSampler()
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
	samplerInfo.unnormalizedCoordinates = VK_FALSE; //Use [0,1] for coordinate not [0, texWidth/texHeight]
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(pCore_->getDevice(), &samplerInfo, nullptr, &sampler_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler");
	}

}


void Texture2D::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(pCore_->getPhysicalDevice(), imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
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

    for (uint32_t i = 1; i < mipLevels; i++)
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
        blit.srcOffsets[0] = { 0,0,0 };
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

        if (mipWidth > 1) { mipWidth /= 2; }
        if (mipHeight > 1) { mipHeight /= 2; }

    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    pCore_->getUtils().endSingleTimeCommands(commandBuffer, false);


}

Texture2D::Texture2D(const VulkanCore * pCore):
    Texture2D(pCore, "", VK_FORMAT_R8G8B8A8_UNORM)
{
}

Texture2D::Texture2D(const VulkanCore* pCore, const std::string& path, const VkFormat& format):
	VkElement(pCore),
    path_(path),
    format_(format)
{
	name_ = "";
}

void Texture2D::create()
{
	createImage();
	createImageView();
	createSampler();
    isCreated_ = true;
}

void Texture2D::destroy()
{
	vkDestroySampler(pCore_->getDevice(), sampler_, nullptr);
	vkDestroyImageView(pCore_->getDevice(), imageView_, nullptr);
	vkDestroyImage(pCore_->getDevice(), image_, nullptr);
	vkFreeMemory(pCore_->getDevice(), imageMemory_, nullptr);
}

void Texture2D::setFilePath(const std::string & path)
{
	path_.assign(path);
}

const VkImage & Texture2D::getImage() const
{
	return image_;
}

const VkImageView & Texture2D::getImageView() const
{
	return imageView_;
}

const VkSampler & Texture2D::getSampler() const
{
	return sampler_;
}


Texture2D::~Texture2D()
{
}


}
