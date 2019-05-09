#pragma once

#include <vector>
#include "VkElement.h"

namespace renderer
{

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentingFamily = -1;
    int transferFamily = -1;

    inline bool isComplete() const
    {
        return graphicsFamily >= 0 && presentingFamily >= 0;
    }

    inline bool transferAvailable() const
    {
        return transferFamily >= 0 && transferFamily != graphicsFamily;
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDeviceProperties
{
private:
    const VkPhysicalDevice* pPhysicalDevice_;
    const VkSurfaceKHR* pSurface_;
    QueueFamilyIndices indices_;
    SwapChainSupportDetails swapChainDetails_;
    VkPhysicalDeviceProperties deviceProperties_;
    VkPhysicalDeviceMemoryProperties memoryProperties_;

    void findQueueFamilies();
    void querySwapChainSupport();

public:
    PhysicalDeviceProperties();
    PhysicalDeviceProperties(const VkPhysicalDevice* pDevice, const VkSurfaceKHR* pSurface);
    void setPhysicalDevice(const VkPhysicalDevice* pDevice);
    void setSurface(const VkSurfaceKHR* pSurface);

    void refreshProperties();
    const QueueFamilyIndices& getQueueFamilyIndices()const;
    const SwapChainSupportDetails& getSwapChainSupportDetails()const;
    const VkPhysicalDeviceProperties& getVkPhysicalDeviceProperties()const;
    const VkPhysicalDeviceMemoryProperties& getVkPhysicalDeviceMemoryProperties()const;

    bool checkDeviceExtensionSupport(const std::vector<const char*>& extensions)const;
    VkFormat findSupportedTilingFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                       VkFormatFeatureFlags features)const;

    ~PhysicalDeviceProperties();
};

}
