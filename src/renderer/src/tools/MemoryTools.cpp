#include "renderer/tools/MemoryTools.h"
#include "renderer/tools/CommandTools.h"

namespace renderer
{

namespace tools
{

namespace memory
{

void createBuffer(const VulkanCore& core, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties,
                  VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    //Create buffer
    VkBufferCreateInfo bufferInfo = {};
    QueueFamilyIndices indices = core.getPhysicalDeviceProperties().getQueueFamilyIndices();
    uint32_t sharingIndices[] = { static_cast<uint32_t>(indices.graphicsFamily), static_cast<uint32_t>(indices.transferFamily) };

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(indices.transferAvailable())
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

    if(vkCreateBuffer(core.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    //Allocate the memory on VRAM
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(core.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(core, memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(core.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    //Bind the memory allocated with the vertex buffer
    vkBindBufferMemory(core.getDevice(), buffer, bufferMemory, 0);
}

void copyBuffer(const VulkanCore& core, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = tools::command::beginSingleTimeTransferCommands(core);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    tools::command::endSingleTimeTransferCommands(core, commandBuffer);
}

/*@brief : Check if the memory type of a device match the requirements in properties
*/
uint32_t findMemoryType(const VulkanCore& core, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties =
        core.getPhysicalDeviceProperties().getVkPhysicalDeviceMemoryProperties();

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if(typeFilter & (1 << i)
           && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}

} // vulkan

} // tools

} // renderer

