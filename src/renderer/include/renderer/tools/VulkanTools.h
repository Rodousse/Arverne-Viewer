#pragma once

#include "renderer/VulkanCore.h"

namespace
{

VkCommandBuffer beginSingleTimeCommands(const renderer::VulkanCore& core, bool useTransfer = false);

void endSingleTimeCommands(const renderer::VulkanCore& core, VkCommandBuffer commandBuffer,
                           bool useTransfer = false);

}

namespace renderer
{

namespace tools
{

namespace vulkan
{

VkCommandBuffer beginSingleTimeCommands(const VulkanCore& core);

VkCommandBuffer beginSingleTimeTransferCommands(const VulkanCore& core);

void endSingleTimeCommands(const VulkanCore& core, VkCommandBuffer commandBuffer);

void endSingleTimeTransferCommands(const VulkanCore& core, VkCommandBuffer commandBuffer);

}

}

}
