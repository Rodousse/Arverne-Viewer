#ifndef DEBUG_MESSENGER_HPP
#define DEBUG_MESSENGER_HPP

#include "VkElement.hpp"
#include <vector>




class DebugMessenger :
	public VkElement
{
private:
	using VkElement::pCore_;

	const VkInstance *pInstance_;
	VkDebugUtilsMessengerEXT messenger_;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
		void* pUserData);


public:
	const std::vector<const char*> VALIDATION_LAYERS = {
		"VK_LAYER_LUNARG_standard_validation"
	};


	DebugMessenger(const VulkanCore* pCore, const VkInstance* instance);


    void create() override;
    void destroy() override;

	bool checkValidationLayerSupport();

    ~DebugMessenger() override;
};



#endif //DEBUG_MESSENGER_HPP
