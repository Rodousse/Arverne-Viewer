#pragma once

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "renderer/DebugMessenger.h"
#include "renderer/PhysicalDeviceProvider.h"
#include "renderer/Swapchain.h"
#include "renderer/VulkanUtils.h"
#include "renderer/Vertex.h"
#include "renderer/texture/MaterialTexture.h"
#include "renderer/camera/Camera.h"
#include "renderer/Model.h"

namespace renderer
{

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

class VulkanCore
{
protected :
    /******************************************* CORE VARIABLE ******************************************************/
    struct ApplicationStateChange
    {
        bool materialModified = false;
        bool modelModified = false;
    };

    struct UniformBufferObject
    {
        glm::mat4x4 model;
        glm::mat4x4 view;
        glm::mat4x4 projection;
        glm::vec3 lightPos;
    };

    const std::vector<const char*> DEVICE_EXTENSIONS =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;
    VkInstance instance_;
    std::vector<const char*> requiredExtensions_;
    VkPhysicalDeviceFeatures requiredDeviceFeatures_;

    PhysicalDeviceProperties physicalDeviceProperties_;
    VkPhysicalDevice physicalDevice_;
    VkDevice logicalDevice_;

    DebugMessenger debugMessenger_;
    VulkanUtils utilities_;

    VkSurfaceKHR surface_;

    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
    VkQueue transfertQueue_;

    VkExtent2D windowExtent_;
    Swapchain swapchain_;

    VkRenderPass renderPass_;
    VkDescriptorSetLayout descriptorSetLayout_;
    VkDescriptorPool descriptorPool_;
    std::vector<VkDescriptorSet> descriptorSets_;
    VkPipelineLayout pipelineLayout_;
    VkPipeline graphicsPipeline_;
    VkViewport viewport_;

    VkCommandPool commandPool_;
    VkCommandPool commandPoolTransfert_;
    std::vector<VkCommandBuffer> commandBuffers_;

    //used to synchronise the image to show
    std::vector<VkSemaphore> imageAvailableSemaphore_; //An image is ready to render
    std::vector<VkSemaphore> renderFinishedSemaphore_; //An image is rendered and wait to be presented
    std::vector<VkFence> inFlightFences_;
    size_t currentFrame_ = 0;

    VkBuffer vertexBuffer_;
    VkDeviceMemory vertexBufferMemory_;
    VkBuffer vertexIndexBuffer_;
    VkDeviceMemory vertexIndexBufferMemory_;
    std::vector<VkBuffer> uniformBuffers_;
    std::vector<VkDeviceMemory> uniformBuffersMemory_;

    VkImage depthImage_;
    VkImageView depthImageView_;
    VkDeviceMemory depthImageMemory_;

    VkImage colorImage_;
    VkDeviceMemory colorMemory_;
    VkImageView colorImageView_;

    VkSampleCountFlagBits msaaSamples_ = VK_SAMPLE_COUNT_1_BIT;

    bool framebufferResize = false;
    bool isCleaned_ = true;

    VkResult areInstanceExtensionsCompatible(const char** extensions, uint32_t extensionsCount);
    VkFormat findDepthFormat();
    VkSampleCountFlagBits getMaxUsableSampleCount();

    /******************************************* APPLICATION VARIABLE ******************************************************/

    MaterialTexture lenaTexture_;
    Camera camera_;
    std::vector<data::Mesh> meshes_;
    Model model_;
    ApplicationStateChange applicationChanges_;

    /***********************************************************************************************************************/

    //Physical devices and Queues compatibility

    void pickPhysicalDevice();
    void createLogicalDevice();

    //Swapchains, graphics pipeline

    void createSwapChain();
    void recreateSwapChain();
    void cleanUpSwapChain();
    void createRenderPass();
    void createGraphicsPipeline();
    void createCommandPool();
    void createCommandBuffers();
    void createDepthRessources();
    void createColorRessources();

    void recreateCommandBuffer();

    //Shader Loading and Creation

    static std::vector<char> readFile(const std::string&  fileName); // Read the content of the spv file
    VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

    // Buffer Management

    void createVertexBuffer();
    void createVertexIndexBuffer();
    void createUniformBuffer();
    void updateUniformBuffer(uint32_t imageIndex);

    //DescriptorSetLayout and Uniform buffer

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();

    // Synchronisation
    void createSyncObjects();
    void checkApplicationState();

    void cleanup();

public:
    VulkanCore();

    const VkSurfaceKHR& getSurface()const;
    const VkInstance& getInstance()const;
    const VkDevice& getDevice()const;
    const VkPhysicalDevice& getPhysicalDevice()const;
    const PhysicalDeviceProperties& getPhysicalDeviceProperties()const;

    const VkQueue& getTransfertQueue()const;
    const VkQueue& getGraphicsQueue()const;

    const VkCommandPool& getCommandPoolTransfer()const;
    const VkCommandPool& getCommandPool()const;

    const VulkanUtils& getUtils()const;

    /******************************************* APPLICATION FUNCTIONS ******************************************************/

    void addRequiredExtensions(const char** extensions,
                               uint32_t extensionCount); ///TODO Call by application
    void initVulkan();
    void setSurface(const VkSurfaceKHR& surface);

    void setPhysicalDeviceFeaturesRequired(VkPhysicalDeviceFeatures features);
    void createInstance();
    void resizeExtent(int width, int height);

    void setCamera(const Camera& camera);
    void setModel(const Model& model);

    void drawFrame();

    /***********************************************************************************************************************/

    ~VulkanCore();
};

}
