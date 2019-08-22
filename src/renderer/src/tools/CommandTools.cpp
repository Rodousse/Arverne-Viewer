#include "renderer/tools/CommandTools.h"
#include "renderer/tools/MemoryTools.h"

namespace
{

VkCommandBuffer beginSingleTimeCommands(const renderer::VulkanCore& core, bool useTransfer)
{
    VkCommandBufferAllocateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferInfo.commandBufferCount = 1;
    bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    bufferInfo.commandPool = useTransfer ? core.getCommandPoolTransfer() : core.getCommandPool();

    VkCommandBuffer commandBuffer;

    vkAllocateCommandBuffers(core.getDevice(), &bufferInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}


void endSingleTimeCommands(const renderer::VulkanCore& core, VkCommandBuffer commandBuffer,
                           bool useTransfer)
{
    VkCommandPool commandPool = useTransfer ? core.getCommandPoolTransfer() :
                                core.getCommandPool();
    VkQueue queue = useTransfer ? core.getTransfertQueue() : core.getGraphicsQueue();

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo subInfo = {};
    subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    subInfo.commandBufferCount = 1;
    subInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &subInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(core.getDevice(), commandPool, 1, &commandBuffer);
}

}

namespace renderer
{

namespace tools
{

namespace command
{

VkCommandBuffer beginSingleTimeCommands(const VulkanCore& core)
{
    return ::beginSingleTimeCommands(core, false);
}

VkCommandBuffer beginSingleTimeTransferCommands(const VulkanCore& core)
{
    return ::beginSingleTimeCommands(core, true);
}

void endSingleTimeCommands(const VulkanCore& core, VkCommandBuffer commandBuffer)
{
    ::endSingleTimeCommands(core, commandBuffer, false);
}

void endSingleTimeTransferCommands(const VulkanCore& core, VkCommandBuffer commandBuffer)
{
    ::endSingleTimeCommands(core, commandBuffer, true);
}



} // vulkan

} // tools

} // renderer

