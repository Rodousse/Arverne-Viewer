#include "renderer/PhysicalDeviceProvider.h"
#include "renderer/VulkanCore.h"

namespace renderer
{

bool PhysicalDeviceProvider::isDeviceSuitable(VkPhysicalDevice device)const
{
    VkPhysicalDeviceProperties deviceProperties = {};
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures = {};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    PhysicalDeviceProperties properties(&device, &pCore_->getSurface());

    QueueFamilyIndices indices = properties.getQueueFamilyIndices();

    bool isDeviceExtensionsSupported = properties.checkDeviceExtensionSupport(neededExtensions_);

    bool swapChainAdequate = false;

    if(isDeviceExtensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = properties.getSwapChainSupportDetails();
        swapChainAdequate = !swapChainSupport.surfaceFormats.empty()
                            && !swapChainSupport.presentModes.empty();
    }

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
           && isDeviceContainingFeatures(deviceFeatures)
           && isDeviceExtensionsSupported
           && swapChainAdequate
           && indices.isComplete();
}

bool PhysicalDeviceProvider::isDeviceContainingFeatures(const VkPhysicalDeviceFeatures& features)
const
{
    for(size_t i = 0; i < sizeof(const VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++)
    {
        const VkBool32* currentFeature = (reinterpret_cast<const VkBool32*>(&features))
                                         + (i * sizeof(VkBool32));
        const VkBool32* currentRequiredFeature = (reinterpret_cast<const VkBool32*>(&requiredFeatures_)) +
                (i * sizeof(VkBool32));

        if(*currentFeature != *currentRequiredFeature && *currentRequiredFeature == VK_TRUE)
        {
            return false;
        }
    }

    return true;
}


PhysicalDeviceProvider::PhysicalDeviceProvider(const VulkanCore* pCore):
    PhysicalDeviceProvider(pCore, {""})
{
}

PhysicalDeviceProvider::PhysicalDeviceProvider(const VulkanCore* pCore,
        const std::vector<const char*> neededExtensions):
    pCore_(pCore)
{
    setNeededDeviceExtensions(neededExtensions);
}

/*  @brief : Get the best physical device following criteria of the application
*   @param physicalDeviceProperties : The properties of the best device will be stored in this variable
*   @retval : VK_NULL_HANDLE if none found, the device otherwise
*/
VkPhysicalDevice PhysicalDeviceProvider::getBestPhysicalDevice()const
{
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    const VkInstance instance = pCore_->getInstance();

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        throw std::runtime_error("No GPU found compatible with vulkan!");
    }

    std::vector<VkPhysicalDevice> availableDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices.data());

    for(const auto& device : availableDevices)
    {
        if(isDeviceSuitable(device))
        {
            bestDevice = device;
            break;
        }
    }

    if(bestDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    return bestDevice;
}



void PhysicalDeviceProvider::setNeededDeviceExtensions(const std::vector<const char*>
        neededExtensions)
{
    neededExtensions_.assign(neededExtensions.begin(), neededExtensions.end());
}

void PhysicalDeviceProvider::setRequiredDeviceFeatures(VkPhysicalDeviceFeatures features)
{
    requiredFeatures_ = features;
}

PhysicalDeviceProvider::~PhysicalDeviceProvider()
{
}


}
