#include "renderer/VulkanApplication.h"

namespace renderer
{


VulkanApplication::VulkanApplication():
    modelManager_(&vkCore_)
{
    createInstance();
}

void VulkanApplication::create(VkSurfaceKHR surface)
{
    vkCore_.setSurface(surface);
    initWindow();
}


ModelManager& VulkanApplication::modelManager()
{
    return modelManager_;
}

void VulkanApplication::setModelManager(const ModelManager& modelManager)
{
    modelManager_ = modelManager;
}

void VulkanApplication::setCamera(const Camera& camera)
{
    vkCore_.setCamera(camera);
}

VkInstance VulkanApplication::vkInstance() const
{
    return vkCore_.getInstance();
}

void VulkanApplication::createInstance()
{
    //Adding required extensions for the application
    std::vector<const char*> extensions;
    //Strictly needed by qt
    extensions.push_back("VK_KHR_surface");
#ifdef UNIX_
    extensions.push_back("VK_KHR_xcb_surface");
#elif WIN32_
    extensions.push_back("VK_KHR_win32_surface");
#endif
    extensions.push_back("VK_EXT_debug_report"); // enable debug as qt log
    vkCore_.addRequiredExtensions(extensions.data(), static_cast<uint32_t>(extensions.size()));


    //Set which device features are needed
    VkPhysicalDeviceFeatures neededFeatures = {};
    neededFeatures.samplerAnisotropy = VK_TRUE;
    neededFeatures.geometryShader = VK_TRUE;
    neededFeatures.sampleRateShading = VK_TRUE;

    vkCore_.setPhysicalDeviceFeaturesRequired(neededFeatures);

    vkCore_.createInstance();
}

void VulkanApplication::initWindow()
{
    vkCore_.resizeExtent(640, 480);
    vkCore_.initVulkan();
    drawFrame();
}

void VulkanApplication::resizeWindow(int width, int height)
{
    if(width != 0 && height != 0)
    {
        vkCore_.resizeExtent(width, height);
    }
}

void VulkanApplication::cleanup()
{
    vkCore_.cleanup();
}

void VulkanApplication::drawFrame()
{
    vkCore_.drawFrame();
}

}
