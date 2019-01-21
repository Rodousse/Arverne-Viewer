#ifndef PHYSICAL_DEVICE_PROVIDER_HPP
#define PHYSICAL_DEVICE_PROVIDER_HPP


#include <vector>
#include "VkElement.hpp"
#include "PhysicalDeviceProperties.hpp"

class PhysicalDeviceProvider
{
private:
	const VulkanCore* pCore_;
	std::vector<const char*> neededExtensions_;
	VkPhysicalDeviceFeatures requiredFeatures_;

	bool isDeviceSuitable(VkPhysicalDevice device)const;
	bool isDeviceContainingFeatures(VkPhysicalDeviceFeatures features)const;
public:
	PhysicalDeviceProvider(const VulkanCore *pCore);
	PhysicalDeviceProvider(const VulkanCore *pCore, const std::vector<const char*> neededExtensions);

	VkPhysicalDevice getBestPhysicalDevice()const; //TODO

	void setNeededDeviceExtensions(const std::vector<const char*> neededExtensions);
	void setRequiredDeviceFeatures(VkPhysicalDeviceFeatures features);

	~PhysicalDeviceProvider();
};



#endif
