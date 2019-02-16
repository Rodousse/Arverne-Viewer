#include "VulkanUtils.hpp"
#include "VulkanCore.hpp"

namespace renderer
{

VulkanUtils::VulkanUtils(const VulkanCore * pCore):
	pCore_(pCore)
{
}


VulkanUtils::~VulkanUtils()
{
}

//------------------------------------------------------------------------------------------------------

VkImageView VulkanUtils::createImageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewCreateFlags flags)const
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

	if (vkCreateImageView(pCore_->getDevice(), &imageViewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view");
	}
	return imageView;
}

//------------------------------------------------------------------------------------------------------

void VulkanUtils::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)const
{
	//Create buffer
	VkBufferCreateInfo bufferInfo = {};
	QueueFamilyIndices indices = pCore_->getPhysicalDeviceProperties().getQueueFamilyIndices();
	uint32_t sharingIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.transferFamily };

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (indices.transferAvailable())
	{
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.pQueueFamilyIndices = sharingIndices;
		bufferInfo.queueFamilyIndexCount = 2;
	}
	else
	{
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.pQueueFamilyIndices = &sharingIndices[0];
		bufferInfo.queueFamilyIndexCount = 1;
	}

	if (vkCreateBuffer(pCore_->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	//Allocate the memory on VRAM
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(pCore_->getDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(pCore_->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	//Bind the memory allocated with the vertex buffer
	vkBindBufferMemory(pCore_->getDevice(), buffer, bufferMemory, 0);
}


//------------------------------------------------------------------------------------------------------

VkCommandBuffer VulkanUtils::beginSingleTimeCommands(bool useTransfer)const
{
	VkCommandBufferAllocateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferInfo.commandBufferCount = 1;
	bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    bufferInfo.commandPool = useTransfer?pCore_->getCommandPoolTransfer():pCore_->getCommandPool();

	VkCommandBuffer commandBuffer;

	vkAllocateCommandBuffers(pCore_->getDevice(), &bufferInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

//------------------------------------------------------------------------------------------------------

void VulkanUtils::endSingleTimeCommands(VkCommandBuffer commandBuffer, bool useTransfer)const
{


    VkCommandPool commandPool = useTransfer?pCore_->getCommandPoolTransfer():pCore_->getCommandPool();
    VkQueue queue = useTransfer?pCore_->getTransfertQueue():pCore_->getGraphicsQueue();

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo subInfo = {};
	subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	subInfo.commandBufferCount = 1;
	subInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &subInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(pCore_->getDevice(), commandPool, 1, &commandBuffer);

}


//------------------------------------------------------------------------------------------------------

void VulkanUtils::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)const
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(true);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, true);

}

//------------------------------------------------------------------------------------------------------


/*@brief : Check if the memory type of a device match the requirements in properties
*
*
*
*/
uint32_t VulkanUtils::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)const
{
	VkPhysicalDeviceMemoryProperties memProperties = pCore_->getPhysicalDeviceProperties().getVkPhysicalDeviceMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i)
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type");
}

//------------------------------------------------------------------------------------------------------

void VulkanUtils::createImage(uint32_t width, uint32_t height,uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags property, VkImage &image, VkDeviceMemory &imageMemory, VkImageCreateFlags flags)const
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

	if (vkCreateImage(pCore_->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements imageMemoryRequirements = {};
	vkGetImageMemoryRequirements(pCore_->getDevice(), image, &imageMemoryRequirements);

	VkMemoryAllocateInfo imageAllocInfo = {};
	imageAllocInfo.allocationSize = imageMemoryRequirements.size;
	imageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageAllocInfo.memoryTypeIndex = findMemoryType(imageMemoryRequirements.memoryTypeBits, property);

	if (vkAllocateMemory(pCore_->getDevice(), &imageAllocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(pCore_->getDevice(), image, imageMemory, 0);
}


void VulkanUtils::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)const
{
    QueueFamilyIndices indices = pCore_->getPhysicalDeviceProperties().getQueueFamilyIndices();
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(true);

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //TODO : Care here, maybe a probleme beetween transfert queue and graphics one
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencilComponent(format))
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        if (indices.transferAvailable())
        {
            barrier.dstQueueFamilyIndex = static_cast<uint32_t>(indices.graphicsFamily);
            barrier.srcQueueFamilyIndex = static_cast<uint32_t>(indices.transferFamily);
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        if (indices.transferAvailable())
        {
            barrier.dstQueueFamilyIndex = static_cast<uint32_t>(indices.graphicsFamily);
            barrier.srcQueueFamilyIndex = static_cast<uint32_t>(indices.transferFamily);
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        if (indices.transferAvailable())
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

    endSingleTimeCommands(commandBuffer, true);
}

//------------------------------------------------------------------------------------------------------

void VulkanUtils::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)const
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(true);
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;
	region.imageOffset = { 0,0,0 };
	region.imageExtent = { width, height, 1 };
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.mipLevel = 0;

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer, true);
}

//------------------------------------------------------------------------------------------------------

VkSampleCountFlagBits VulkanUtils::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties = pCore_->getPhysicalDeviceProperties().getVkPhysicalDeviceProperties();

    VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    if(counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;
    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;
    if (counts &  VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;
    if (counts &  VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;
    if (counts &  VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;


    return VK_SAMPLE_COUNT_1_BIT;
}

//------------------------------------------------------------------------------------------------------

bool VulkanUtils::hasStencilComponent(VkFormat format)const
{
    return format == VK_FORMAT_D16_UNORM_S8_UINT
        || format == VK_FORMAT_D24_UNORM_S8_UINT
        || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}


}
