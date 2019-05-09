
#include <exception>
#include <iostream>

#include "renderer/DebugMessenger.h"


namespace renderer
{

VkResult CreateDebugUtilMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                     VkAllocationCallbacks* pAllocator,
                                     VkDebugUtilsMessengerEXT* pCallback);


VkResult DestroyDebugUtilMessengerEXT(VkInstance instance,
                                      VkDebugUtilsMessengerEXT messenger,
                                      const VkAllocationCallbacks* pAllocator);


DebugMessenger::DebugMessenger(const VulkanCore* pCore, const VkInstance* instance) :
    VkElement(pCore),
    pInstance_(instance)
{

}

DebugMessenger::~DebugMessenger()
{
}

void DebugMessenger::create()
{
    std::cout << "Set up debug callback" << std::endl;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
    debugInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallback;
    debugInfo.pUserData = nullptr;

    if(CreateDebugUtilMessengerEXT(*pInstance_, &debugInfo, nullptr, &messenger_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up Debug Callback !");
    }
}

void DebugMessenger::destroy()
{
    if(DestroyDebugUtilMessengerEXT(*pInstance_, messenger_, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Impossible to destroy validation layer messenger!");
    }
}

/*  @brief : Function callBack, called when a debug message coming from vulkan
*   @param messageSeverity :
*       VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
*       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
*       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
*       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
*
*   @param messageType :
*       VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
*       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
*       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
*
*   @param pCallBackData :
*       pMessage : contains the message as a string
*       pObjects : the objects associated with it
*       objectCount : The number of objects associated
*
*   @param pUserData :
*       A pointer specified during the setup of the callBack. It could be anything !
*/
VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
    void* pUserData)
{
    (pCallBackData->pMessage);
    return VK_FALSE;
}

/* @brief : This will look for the function we need to create a messenger
*/
VkResult CreateDebugUtilMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                     VkAllocationCallbacks* pAllocator,
                                     VkDebugUtilsMessengerEXT* pCallback)
{
    auto createMessengerFunct = (PFN_vkCreateDebugUtilsMessengerEXT)
                                vkGetInstanceProcAddr(instance,
                                        "vkCreateDebugUtilsMessengerEXT");

    if(createMessengerFunct != nullptr)
    {
        return createMessengerFunct(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/* @brief : This will look for the function we need to destroy and deallocate the messenger
*           we specified in the parameters
*/
VkResult DestroyDebugUtilMessengerEXT(VkInstance instance,
                                      VkDebugUtilsMessengerEXT messenger,
                                      const VkAllocationCallbacks* pAllocator)
{
    auto destroyMessengerFunct = (PFN_vkDestroyDebugUtilsMessengerEXT)
                                 vkGetInstanceProcAddr(instance,
                                         "vkDestroyDebugUtilsMessengerEXT");

    if(destroyMessengerFunct != nullptr)
    {
        destroyMessengerFunct(instance, messenger, pAllocator);
        return VK_SUCCESS;
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

bool DebugMessenger::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : VALIDATION_LAYERS)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
        {
            return false;
        }
    }

    return true;
}

}
