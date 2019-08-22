#pragma once

#include "renderer/VulkanCore.h"

namespace renderer
{

namespace tools
{

namespace memory
{

void createBuffer(const VulkanCore& core, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties,
                  VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void copyBuffer(const VulkanCore& core, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

/*@brief : Check if the memory type of a device match the requirements in properties
*/
uint32_t findMemoryType(const VulkanCore& core, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

}

}

}
