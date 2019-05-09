#include "renderer/PhysicalDeviceProperties.h"
#include <set>
#include <string>

namespace renderer
{

void PhysicalDeviceProperties::refreshProperties()
{
    if(pSurface_ != nullptr && pPhysicalDevice_ != nullptr)
    {
        findQueueFamilies();
        querySwapChainSupport();
        vkGetPhysicalDeviceProperties(*pPhysicalDevice_, &deviceProperties_);
        vkGetPhysicalDeviceMemoryProperties(*pPhysicalDevice_, &memoryProperties_);
    }
}

void PhysicalDeviceProperties::findQueueFamilies()
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice_, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice_, &queueFamilyCount,
            queueProperties.data());

    uint32_t index = 0;

    for(const auto& queueProperty : queueProperties)
    {
        if(queueProperty.queueCount > 0 && queueProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = index;
        }

        if(queueProperty.queueCount > 0 && queueProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            indices.transferFamily = index;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(*pPhysicalDevice_, index, *pSurface_, &presentSupport);

        if(queueProperty.queueCount > 0 && presentSupport)
        {
            indices.presentingFamily = index;
        }

        if(indices.isComplete() && indices.transferAvailable())
        {
            break;
        }

        index++;
    }

    indices_ = indices;
}

void PhysicalDeviceProperties::querySwapChainSupport()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pPhysicalDevice_, *pSurface_,
            &swapChainDetails_.surfaceCapabilities);

    uint32_t surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice_, *pSurface_, &surfaceFormatCount, nullptr);

    if(surfaceFormatCount != 0)
    {
        swapChainDetails_.surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice_, *pSurface_, &surfaceFormatCount,
                                             swapChainDetails_.surfaceFormats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*pPhysicalDevice_, *pSurface_, &presentModeCount,
            nullptr);

    if(presentModeCount != 0)
    {
        swapChainDetails_.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(*pPhysicalDevice_, *pSurface_, &presentModeCount,
                swapChainDetails_.presentModes.data());
    }

}


bool PhysicalDeviceProperties::checkDeviceExtensionSupport(const std::vector<const char*>&
        extensions)const
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(*pPhysicalDevice_, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensionsProperties(extensionCount);
    vkEnumerateDeviceExtensionProperties(*pPhysicalDevice_, nullptr, &extensionCount,
                                         extensionsProperties.data());

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    for(const auto& extension : extensionsProperties)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}


VkFormat PhysicalDeviceProperties::findSupportedTilingFormat(const std::vector<VkFormat>&
        candidates, VkImageTiling tiling, VkFormatFeatureFlags features)const
{
    for(VkFormat format : candidates)
    {
        VkFormatProperties formatProp;
        vkGetPhysicalDeviceFormatProperties(*pPhysicalDevice_, format, &formatProp);

        if(tiling == VK_IMAGE_TILING_LINEAR && (formatProp.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if(tiling == VK_IMAGE_TILING_OPTIMAL
                && (formatProp.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find a supported format!");
}


PhysicalDeviceProperties::PhysicalDeviceProperties():
    PhysicalDeviceProperties(nullptr, nullptr)
{
}

PhysicalDeviceProperties::PhysicalDeviceProperties(const VkPhysicalDevice* pDevice,
        const VkSurfaceKHR* pSurface):
    pPhysicalDevice_(pDevice),
    pSurface_(pSurface)
{
    refreshProperties();
}

void PhysicalDeviceProperties::setPhysicalDevice(const VkPhysicalDevice* pDevice)
{
    pPhysicalDevice_ = pDevice;
    refreshProperties();
}

void PhysicalDeviceProperties::setSurface(const VkSurfaceKHR* pSurface)
{
    pSurface_ = pSurface;
    refreshProperties();
}

const QueueFamilyIndices& PhysicalDeviceProperties::getQueueFamilyIndices()const
{
    return indices_;
}

const SwapChainSupportDetails& PhysicalDeviceProperties::getSwapChainSupportDetails()const
{
    return swapChainDetails_;
}

const VkPhysicalDeviceProperties& PhysicalDeviceProperties::getVkPhysicalDeviceProperties()const
{
    return deviceProperties_;
}

const VkPhysicalDeviceMemoryProperties&
PhysicalDeviceProperties::getVkPhysicalDeviceMemoryProperties()
const
{
    return memoryProperties_;
}


PhysicalDeviceProperties::~PhysicalDeviceProperties()
{
}

}
