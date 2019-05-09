#pragma once

#include <vector>
#include "VkElement.h"
#include "PhysicalDeviceProperties.h"

namespace renderer
{


class PhysicalDeviceProvider
{
private:
    const VulkanCore* pCore_;
    std::vector<const char*> neededExtensions_;
    VkPhysicalDeviceFeatures requiredFeatures_;

    bool isDeviceSuitable(VkPhysicalDevice device)const;
    bool isDeviceContainingFeatures(const VkPhysicalDeviceFeatures& features)const;
public:
    PhysicalDeviceProvider(const VulkanCore* pCore);
    PhysicalDeviceProvider(const VulkanCore* pCore, const std::vector<const char*> neededExtensions);

    VkPhysicalDevice getBestPhysicalDevice()const; //TODO

    void setNeededDeviceExtensions(const std::vector<const char*> neededExtensions);
    void setRequiredDeviceFeatures(VkPhysicalDeviceFeatures features);

    ~PhysicalDeviceProvider();
};

}
