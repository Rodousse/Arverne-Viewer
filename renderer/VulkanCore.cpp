#include "VulkanCore.hpp"
#include <algorithm>
#include <set>
#include <array>
#include <fstream>
#include <chrono>
#include <QFile>
#include "loader/ObjLoader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace renderer
{

VulkanCore::VulkanCore():
	debugMessenger_(this, &instance_),
	swapchain_(this),
	utilities_(this),
    lenaTexture_(this, ":/textures/default.bmp", VK_FORMAT_R8G8B8A8_UNORM),
  model_(this)
{

	if (ENABLE_VALIDATION_LAYERS)
	{
		requiredExtensions_.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

}

//------------------------------------------------------------------------------------------------------

void VulkanCore::drawFrame()
{
    checkApplicationState();

	uint32_t imageIndex;

	vkWaitForFences(logicalDevice_, 1, &inFlightFences_[currentFrame_], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(logicalDevice_, 1, &inFlightFences_[currentFrame_]);
	//Timeout in nanoseconds = numeric_limits... using the max disable the timeout
	VkResult result = vkAcquireNextImageKHR(logicalDevice_, swapchain_.getVkSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore_[currentFrame_], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

    updateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore_[currentFrame_];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore_[currentFrame_];

	VkPipelineStageFlags waitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStageFlags; //We wait the semaphore with to have the entry with the same index 
												//Here we wait the VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT of the imageAvailableSemaphore_

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers_[imageIndex];

	vkResetFences(logicalDevice_, 1, &inFlightFences_[currentFrame_]);
	if (vkQueueSubmit(presentQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}


	VkResult presentatioResult;
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain_.getVkSwapchain();
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore_[currentFrame_];
	presentInfo.pResults = &presentatioResult; //Array of results for each swap chain images

	result = vkQueuePresentKHR(presentQueue_, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResize)
	{
		framebufferResize = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	vkQueueWaitIdle(presentQueue_);

	currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}

VulkanCore::~VulkanCore()
{
	cleanup();
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::addRequiredExtensions(const char** extensions, uint32_t extensionCount)
{
	requiredExtensions_.insert(requiredExtensions_.end(), extensions, extensions+extensionCount);
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::initVulkan()
{
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
    createCommandPool();
    createDepthRessources();
    createColorRessources();
    swapchain_.createFramebuffers(renderPass_, {colorImageView_, depthImageView_});
	lenaTexture_.create();
//	createVertexBuffer();
//	createVertexIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();

    std::cout << "Vulkan Initialisation Finished" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::setSurface(const VkSurfaceKHR &surface)
{
    surface_ = surface;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::setPhysicalDeviceFeaturesRequired(VkPhysicalDeviceFeatures features)
{
	requiredDeviceFeatures_ = features;
}

//------------------------------------------------------------------------------------------------------

const VkSurfaceKHR & VulkanCore::getSurface()const
{
	return surface_;
}

//------------------------------------------------------------------------------------------------------

const VkInstance& VulkanCore::getInstance() const
{
	return instance_;
}

//------------------------------------------------------------------------------------------------------

const VkDevice & VulkanCore::getDevice()const
{
    return logicalDevice_;
}

//------------------------------------------------------------------------------------------------------

const VkPhysicalDevice &VulkanCore::getPhysicalDevice() const
{
    return physicalDevice_;
}

//------------------------------------------------------------------------------------------------------

const PhysicalDeviceProperties & VulkanCore::getPhysicalDeviceProperties()const
{
	return physicalDeviceProperties_;
}

//------------------------------------------------------------------------------------------------------

const VkQueue & VulkanCore::getTransfertQueue() const
{
	QueueFamilyIndices indices = physicalDeviceProperties_.getQueueFamilyIndices();
	if (indices.transferAvailable())
	{
		return transfertQueue_;
	}
	else
	{
		return getGraphicsQueue();
	}
}

//------------------------------------------------------------------------------------------------------

const VkQueue & VulkanCore::getGraphicsQueue() const
{
	return graphicsQueue_;
}

//------------------------------------------------------------------------------------------------------

const VkCommandPool & VulkanCore::getCommandPoolTransfer() const
{
	QueueFamilyIndices indices = physicalDeviceProperties_.getQueueFamilyIndices();
	if (indices.transferAvailable())
	{
		return commandPoolTransfert_;
	}
	else
	{
		return getCommandPool();
	}
}

//------------------------------------------------------------------------------------------------------

const VkCommandPool & VulkanCore::getCommandPool() const
{
	return commandPool_;
}

//------------------------------------------------------------------------------------------------------

const VulkanUtils & VulkanCore::getUtils() const
{
	return utilities_;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createInstance()
{
    isCleaned_ = false;
	//check if the validation layers are needed and if they are available
	if (ENABLE_VALIDATION_LAYERS && !debugMessenger_.checkValidationLayerSupport())
	{
		throw std::runtime_error("Validation layer requested, but not available !");
	}

	if (areInstanceExtensionsCompatible(requiredExtensions_.data(), static_cast<uint32_t>(requiredExtensions_.size())) != VK_SUCCESS)
	{
		throw std::runtime_error("Some Extensions are not compatible !");
	}

	std::cout << "Vulkan Instance Creation..." << std::endl;
	//Applications infos about the version, the engine version, the vulkan version used
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "PBR Viewer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "SuperViewerArverne";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Informations relative to the appInfo and the window system used
	//And need the extensions to use from the window system (dependent of the library used)
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Validation layer precisions passed to the instance
	if (ENABLE_VALIDATION_LAYERS)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(debugMessenger_.VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = debugMessenger_.VALIDATION_LAYERS.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions_.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions_.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Instance !");
	}

	if (ENABLE_VALIDATION_LAYERS)
	{
		debugMessenger_.create();
	}

	std::cout << "Vulkan Instance Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::resizeExtent(int width, int height)
{
	framebufferResize = true;
	windowExtent_.width = width;
	windowExtent_.height = height;
    physicalDeviceProperties_.refreshProperties(); //Used for querySwapChainSupport
}

void VulkanCore::setCamera(const Camera &camera)
{
    camera_ = camera;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::setModel(const Model &model)
{
    applicationChanges_.modelModified = true;
    model_.destroy();
    model_ = model;
    model_.create();
}

//------------------------------------------------------------------------------------------------------

VkResult VulkanCore::areInstanceExtensionsCompatible(const char** extensions, uint32_t extensionsCount)
{
	//How many extensions are available with vulkan
	uint32_t vkExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

	//Store them
	std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());

	std::cout << "Number of vulkan extensions available " << vkExtensionCount << std::endl;
	for (const auto& extension : vkExtensions)
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	//If an extension passed in the parameter is not in the list of the vulkan available extensions, we can't create the instance 
	for (uint8_t i = 0; i < extensionsCount; i++)
	{
		if (std::find_if(vkExtensions.begin(), vkExtensions.end(), [extensions, i](const VkExtensionProperties& prop)
		{
			return strcmp(prop.extensionName, extensions[i]) == 0;
		}) == vkExtensions.end())
		{
			std::cerr << "Extension : " << extensions[i] << " not supported" << std::endl;
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
    return VK_SUCCESS;
}

//------------------------------------------------------------------------------------------------------

VkFormat VulkanCore::findDepthFormat()
{
    return physicalDeviceProperties_.findSupportedTilingFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::pickPhysicalDevice()
{
	std::cout << "Picking a physical device" << std::endl;
	PhysicalDeviceProvider phyProvider(this, DEVICE_EXTENSIONS);

	phyProvider.setRequiredDeviceFeatures(requiredDeviceFeatures_);

	physicalDevice_ = phyProvider.getBestPhysicalDevice();

	physicalDeviceProperties_.setPhysicalDevice(&physicalDevice_);
	physicalDeviceProperties_.setSurface(&surface_);
    msaaSamples_ = getMaxUsableSampleCount();
	std::cout << physicalDeviceProperties_.getVkPhysicalDeviceProperties().deviceName << " : I chose you !!!!" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createLogicalDevice()
{
	std::cout << "Creating a logical device..." << std::endl;
	QueueFamilyIndices indices = physicalDeviceProperties_.getQueueFamilyIndices();
	float queuePriority = 1.0f;

	//Specifying the queues to be created
	std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;

	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentingFamily , indices.transferFamily };
	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queuesCreateInfo.push_back(queueCreateInfo);
	}

	//logical device creation
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	if (ENABLE_VALIDATION_LAYERS)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(debugMessenger_.VALIDATION_LAYERS.size());
		deviceCreateInfo.ppEnabledLayerNames = debugMessenger_.VALIDATION_LAYERS.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}
	deviceCreateInfo.pQueueCreateInfos = queuesCreateInfo.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queuesCreateInfo.size());
	deviceCreateInfo.pEnabledFeatures = &requiredDeviceFeatures_; //Specify device features
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	deviceCreateInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

	if (vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice_)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice_, indices.graphicsFamily, 0, &graphicsQueue_);
	vkGetDeviceQueue(logicalDevice_, indices.presentingFamily, 0, &presentQueue_);
	vkGetDeviceQueue(logicalDevice_, indices.transferFamily, 0, &transfertQueue_);

	std::cout << "Logical device created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createSwapChain()
{
	std::cout << "Swapchain Creation..." << std::endl;

	swapchain_.setExtent(windowExtent_);
	swapchain_.create();

	std::cout << "Swapchain created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createRenderPass()
{
	std::cout << "Creating Render Pass..." << std::endl;



    VkAttachmentDescription colorAttachment = {};

    colorAttachment.format = swapchain_.getFormat().format;
    colorAttachment.samples = msaaSamples_;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear the color to constant value at start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Rendered contents will be stored in memory and can be read later
    //We don't use stencil at the moment
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //Define which layout we have before render pass
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //and at the end
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;



    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = msaaSamples_;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



    VkAttachmentDescription colorAttachmentResolve = {};
    colorAttachmentResolve.format = swapchain_.getFormat().format;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef = {};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	// The index of the  attachement is the directive from :
	// layout(location = "index") out vec4 color; 
	subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;


	VkSubpassDependency subPassDep = {};
	subPassDep.srcSubpass = VK_SUBPASS_EXTERNAL; //Refers to the implicit subpass before the render pass (It it was in dstSubpass would be after the render pass)
	subPassDep.dstSubpass = 0;
	subPassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //Wait for the swap chain to read the image
	subPassDep.srcAccessMask = 0;
	//The operation which should wait this subpass are read and write operation on color attachement
	subPassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subPassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;



    std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &subPassDep;

	if (vkCreateRenderPass(logicalDevice_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Render pass!");
	}

	std::cout << "Render Pass Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createDescriptorSetLayout()
{
	std::cout << "Creating Descriptor Set Layout..." << std::endl;
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0; //Value in shader "layout(binding = 0) uniform"
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1; //Number of object to pass
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1; //Number of object to pass
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;


	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(logicalDevice_, &layoutInfo, nullptr, &descriptorSetLayout_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	std::cout << "Descriptor Set Layout Created" << std::endl;

}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createGraphicsPipeline()
{
	std::cout << "Creating Graphics Pipeline..." << std::endl;
    auto vertexShader = readFile(":/shaders/vert.spv");
    auto fragmentShader = readFile(":/shaders/frag.spv");

	VkShaderModule vertexShaderModule = createShaderModule(vertexShader);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.module = vertexShaderModule;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr;	//Can configure constant values in shader code, 
														//which is more optimized than have a configurable value at runtime

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.module = fragmentShaderModule;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStageInfos[] = { vertShaderStageInfo, fragShaderStageInfo };


	//describe the infos inputed for the vertex and the structure of the datas (size, offset,...)
	auto vertexBindingDescription = Vertex::getBindingDescription();
	auto vertexAttributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescription;
	vertexInputInfo.vertexBindingDescriptionCount = 1;

	VkPipelineInputAssemblyStateCreateInfo assemblyInfos = {};
	assemblyInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyInfos.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyInfos.primitiveRestartEnable = VK_FALSE;

	viewport_ = {};
	viewport_.x = 0;
	viewport_.y = 0;
	viewport_.width = (float)swapchain_.getExtent().width;
	viewport_.height = (float)swapchain_.getExtent().height;
	viewport_.minDepth = 0.0f;
	viewport_.maxDepth = 1.0f;

	//The scissor is masking the "out of the scissor rectangle" data from the viewport
	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain_.getExtent();

	VkPipelineViewportStateCreateInfo viewPortInfo = {};
	viewPortInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewPortInfo.viewportCount = 1;
	viewPortInfo.pScissors = &scissor;
	viewPortInfo.scissorCount = 1;
	viewPortInfo.pViewports = &viewport_;

	VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerInfo.lineWidth = 1.0f;
	rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;// VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerInfo.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multiSampInfo = {};
	multiSampInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampInfo.sampleShadingEnable = VK_TRUE;
    multiSampInfo.rasterizationSamples = msaaSamples_;
    multiSampInfo.minSampleShading = 0.2f;


    VkPipelineDepthStencilStateCreateInfo stencilInfos = {};
    stencilInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    stencilInfos.depthCompareOp = VK_COMPARE_OP_LESS;
    stencilInfos.depthTestEnable = VK_TRUE;
    stencilInfos.depthWriteEnable = VK_TRUE;

	//Color blend describe how we want to replace the color in the current framebuffer
	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend.blendEnable = VK_FALSE;
	colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlend.colorBlendOp = VK_BLEND_OP_ADD; //Seems to have pretty cool fast features here
	colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.pAttachments = &colorBlend;
	colorBlendInfo.logicOpEnable = VK_FALSE; // Use for bitwise operation
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendInfo.attachmentCount = 1;
	//colorBlendInfo.blendConstants[0...4] = floatValue; 

	//This set which of the previous value can be dynamically change during runtime !!!
	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pDynamicStates = dynamicStates;
	dynamicStateInfo.dynamicStateCount = 1;


	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(logicalDevice_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}



	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStageInfos;
	pipelineInfo.layout = pipelineLayout_;
	pipelineInfo.pMultisampleState = &multiSampInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pDepthStencilState = &stencilInfos;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.pInputAssemblyState = &assemblyInfos;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pViewportState = &viewPortInfo;
	pipelineInfo.renderPass = renderPass_;
	pipelineInfo.subpass = 0;
	//We can make pipeline derivates from other pipelines if they have similarities
	//We have a single pipeline here, so we don't use it
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(logicalDevice_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(logicalDevice_, vertexShaderModule, nullptr);
	vkDestroyShaderModule(logicalDevice_, fragmentShaderModule, nullptr);

	std::cout << "Graphics Pipeline Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createCommandPool()
{
	std::cout << "Creating Command Pools..." << std::endl;
    QueueFamilyIndices indices = physicalDeviceProperties_.getQueueFamilyIndices();


	//Create a command pool only for graphics operations
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = indices.graphicsFamily;

	if (vkCreateCommandPool(logicalDevice_, &commandPoolInfo, nullptr, &commandPool_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Command Pool!");
	}

    if (indices.transferAvailable())
	{
        commandPoolInfo.queueFamilyIndex = indices.transferFamily;
		if (vkCreateCommandPool(logicalDevice_, &commandPoolInfo, nullptr, &commandPoolTransfert_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create transfert command pool!");
		}
	}

	std::cout << "Command Pools Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createDepthRessources()
{
    VkFormat depthFormat = findDepthFormat();
    utilities_.createImage(swapchain_.getExtent().width, swapchain_.getExtent().height, 1, msaaSamples_, depthFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage_, depthImageMemory_);

    depthImageView_ = utilities_.createImageView(depthFormat, depthImage_, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    utilities_.transitionImageLayout(depthImage_, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);


}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createColorRessources()
{
    VkFormat format = swapchain_.getFormat().format;

    utilities_.createImage(swapchain_.getExtent().width, swapchain_.getExtent().height, 1,
        msaaSamples_, format, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage_, colorMemory_);
    colorImageView_ = utilities_.createImageView(format, colorImage_, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    utilities_.transitionImageLayout(colorImage_, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);

}

void VulkanCore::recreateCommandBuffer()
{
    vkDeviceWaitIdle(logicalDevice_);
    vkFreeCommandBuffers(logicalDevice_, commandPool_, (uint32_t)commandBuffers_.size(), commandBuffers_.data());
    createCommandBuffers();

}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createVertexBuffer()
{
	std::cout << "Creating and Allocating Vertex Buffer" << std::endl;
    VkDeviceSize bufferSize = sizeof(meshes_[0].vertices[0]) * meshes_[0].vertices.size();
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	utilities_.createBuffer(bufferSize, usage, properties, stagingBuffer, stagingBufferMemory);

	void* pData;//Contains a pointer to the mapped memory

	//Documentation : memory must have been created with a memory type that reports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	//					flags is reserved for future use of the vulkanAPI.
	vkMapMemory(logicalDevice_, stagingBufferMemory, 0, bufferSize, 0, &pData);
    memcpy(pData, meshes_[0].vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice_, stagingBufferMemory);

	utilities_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);
	utilities_.copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);

	vkDestroyBuffer(logicalDevice_, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice_, stagingBufferMemory, nullptr);
	std::cout << "Vertex Buffer Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createVertexIndexBuffer()
{

	std::cout << "Creating and Allocating Index Buffer" << std::endl;
    VkDeviceSize bufferSize = sizeof(meshes_[0].indices[0]) * meshes_[0].indices.size();
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	utilities_.createBuffer(bufferSize, usage, properties, stagingBuffer, stagingBufferMemory);

	void* pData;//Contains a pointer to the mapped memory

	//Documentation : memory must have been created with a memory type that reports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	//					flags is reserved for future use of the vulkanAPI.
	vkMapMemory(logicalDevice_, stagingBufferMemory, 0, bufferSize, 0, &pData);
    memcpy(pData, meshes_[0].indices.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice_, stagingBufferMemory);

	utilities_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexIndexBuffer_, vertexIndexBufferMemory_);
	utilities_.copyBuffer(stagingBuffer, vertexIndexBuffer_, bufferSize);

	vkDestroyBuffer(logicalDevice_, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice_, stagingBufferMemory, nullptr);
	std::cout << "Index Buffer Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createUniformBuffer()
{
	std::cout << "Creating Uniform Buffer..." << std::endl;
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers_.resize(swapchain_.getImageViews().size());
	uniformBuffersMemory_.resize(swapchain_.getImageViews().size());

	for (size_t i = 0; i < swapchain_.getImageViews().size(); i++)
	{
		utilities_.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers_[i], uniformBuffersMemory_[i]);
	}
	std::cout << "Uniform Buffer Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::updateUniformBuffer(uint32_t imageIndex)
{
    uint16_t angle = 0;

    UniformBufferObject ubo = {};

    ubo.model = glm::mat4x4(1.0f);
    ubo.view = glm::mat4x4(1.0f);

    ubo.view = glm::lookAt(camera_.getPosition(), camera_.getCenter(), camera_.getUp());


    ubo.projection = glm::perspective(camera_.getFov(), swapchain_.getExtent().width / (float)swapchain_.getExtent().height, 0.01f, 100.0f);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = 2*std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.lightPos = glm::vec3(4*cos(time), 4*sin(time), 3);

    void *pData;
    for(uint8_t i = 0; i < swapchain_.getImageViews().size(); i++)
    {
        vkMapMemory(logicalDevice_, uniformBuffersMemory_[i], 0, sizeof (UniformBufferObject), 0, &pData);
        memcpy(pData, &ubo, sizeof(UniformBufferObject));
        vkUnmapMemory(logicalDevice_, uniformBuffersMemory_[i]);
    }
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createDescriptorPool()
{
	std::cout << "Creating Descriptor Pool..." << std::endl;

    std::array<VkDescriptorPoolSize, 2> descPoolSizes = {};
	//UBO
	descPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descPoolSizes[0].descriptorCount = static_cast<uint32_t>(swapchain_.getImages().size());

	//Textures
    descPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descPoolSizes[1].descriptorCount = static_cast<uint32_t>(swapchain_.getImages().size());


	VkDescriptorPoolCreateInfo descPoolInfo = {};
	descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolInfo.poolSizeCount = static_cast<uint32_t>(descPoolSizes.size());
	descPoolInfo.pPoolSizes = descPoolSizes.data();
	descPoolInfo.maxSets = static_cast<uint32_t>(swapchain_.getImages().size());

	if (vkCreateDescriptorPool(logicalDevice_, &descPoolInfo, nullptr, &descriptorPool_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
	std::cout << "Descriptor Pool Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createDescriptorSets()
{
	std::cout << "Creating Descriptor Sets..." << std::endl;
	std::vector<VkDescriptorSetLayout> layouts(swapchain_.getImages().size(), descriptorSetLayout_);
	VkDescriptorSetAllocateInfo descAlloc = {};
	descAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descAlloc.descriptorPool = descriptorPool_;
	descAlloc.descriptorSetCount = static_cast<uint32_t>(swapchain_.getImages().size());
	descAlloc.pSetLayouts = layouts.data();

	descriptorSets_.resize(swapchain_.getImages().size());
	if (vkAllocateDescriptorSets(logicalDevice_, &descAlloc, descriptorSets_.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapchain_.getImages().size(); i++)
	{
		VkDescriptorBufferInfo descBufferInfo = {};
		descBufferInfo.buffer = uniformBuffers_[i];
		descBufferInfo.offset = 0;
		descBufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = lenaTexture_.getImageView();
		imageInfo.sampler = lenaTexture_.getSampler();

		std::array<VkWriteDescriptorSet, 2> writeInfos = {};
		writeInfos[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfos[0].dstSet = descriptorSets_[i];
		writeInfos[0].dstBinding = 0; //binding index in "layout(binding = 0)"
		writeInfos[0].dstArrayElement = 0;
		writeInfos[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeInfos[0].descriptorCount = 1; //We can update multiple descriptor at once in an array
		writeInfos[0].pBufferInfo = &descBufferInfo;

		writeInfos[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfos[1].dstSet = descriptorSets_[i];
		writeInfos[1].dstBinding = 1; //binding index in "layout(binding = 0)"
		writeInfos[1].dstArrayElement = 0;
		writeInfos[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeInfos[1].descriptorCount = 1; //We can update multiple descriptor at once in an array
		writeInfos[1].pImageInfo = &imageInfo;


		vkUpdateDescriptorSets(logicalDevice_, static_cast<uint32_t>(writeInfos.size()), writeInfos.data(), 0, nullptr);
	}

	std::cout << "Descriptor Sets Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

/*@brief :	Create the command buffers associated with the command pool
*			We have a command buffer for every image views in the swapchain
*			to render each one of them
*/
void VulkanCore::createCommandBuffers()
{

	std::cout << "Creating and Recording Command Buffers..." << std::endl;
	commandBuffers_.resize(swapchain_.getFramebuffers().size());

	VkCommandBufferAllocateInfo allocateBufferInfo = {};
	allocateBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateBufferInfo.commandPool = commandPool_;
	allocateBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //Can be submitted to queue, and SECONDARY can be called inside a primary command buffer
	allocateBufferInfo.commandBufferCount = (uint32_t)commandBuffers_.size();

	if (vkAllocateCommandBuffers(logicalDevice_, &allocateBufferInfo, commandBuffers_.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command buffers!");
	}

	//For every command buffer we begin recording it, but we have to specify how
	for (size_t i = 0; i < commandBuffers_.size(); i++)
	{
		VkCommandBufferBeginInfo commandBeginInfo = {};
		commandBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; //We can resubmit the command buffer while it's already pending execution
		commandBeginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers_[i], &commandBeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 1.0f, (153.0f/255.0f), (51.0f/255.0f), 1.0f };
        clearValues[1].depthStencil = { 1.0,0 };

		VkRenderPassBeginInfo renderBeginInfo = {};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderBeginInfo.pClearValues = clearValues.data();
		renderBeginInfo.renderPass = renderPass_;
		renderBeginInfo.framebuffer = swapchain_.getFramebuffers()[i];
		renderBeginInfo.renderArea.extent = swapchain_.getExtent();
		renderBeginInfo.renderArea.offset = { 0,0 };

		vkCmdBeginRenderPass(commandBuffers_[i], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // Last parameter used to embedd the command for a primary command buffer or secondary

            for(uint32_t idxMesh = 0;  idxMesh < model_.getMeshes().size(); idxMesh++)
            {
                const MeshData &meshData = model_.getMeshData()[idxMesh];
                const Mesh &mesh = model_.getMeshes()[idxMesh];
                vkCmdBindPipeline(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);

                VkBuffer vertexBuffers[] = { meshData.vertexBuffer };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(commandBuffers_[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers_[i], meshData.vertexIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindDescriptorSets(commandBuffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSets_[i], 0, nullptr);

                //1 used for the instanced rendering could be higher i think for multiple instanced
                vkCmdDrawIndexed(commandBuffers_[i], static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
            }

		vkCmdEndRenderPass(commandBuffers_[i]);

		if (vkEndCommandBuffer(commandBuffers_[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

	}
	std::cout << "Command Buffers Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::createSyncObjects()
{

	std::cout << "Creating Synchronization Objects..." << std::endl;
	imageAvailableSemaphore_.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphore_.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(logicalDevice_, &semaphoreInfo, nullptr, &imageAvailableSemaphore_[i]) != VK_SUCCESS
			|| vkCreateSemaphore(logicalDevice_, &semaphoreInfo, nullptr, &renderFinishedSemaphore_[i]) != VK_SUCCESS
			|| vkCreateFence(logicalDevice_, &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphore for a frame!");
		}
	}


    std::cout << "Synchronization Objects Created" << std::endl;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::checkApplicationState()
{
    for(size_t idx = 0; idx < sizeof (ApplicationStateChange); idx += sizeof(bool))
    {
        bool* state = (reinterpret_cast<bool*>(&applicationChanges_) + idx);
        if(state == &applicationChanges_.modelModified && *state){
            recreateCommandBuffer();
            *state = false;
            return;
        }
        else if(state == &applicationChanges_.modelModified && state){
            return;
        }
    }
}

//------------------------------------------------------------------------------------------------------

std::vector<char> VulkanCore::readFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::OpenModeFlag::ReadOnly))
    {
        qFatal("Failed to read shader %s", qPrintable(fileName));
    }
    QByteArray fileBytes = file.readAll();
    std::vector<char> fileContent(fileBytes.size());
    memcpy(fileContent.data(), fileBytes.data(), fileBytes.size());
    file.close();

	return fileContent;
}

//------------------------------------------------------------------------------------------------------

VkShaderModule VulkanCore::createShaderModule(const std::vector<char>& shaderCode)
{
	std::cout << "Creating Shader Modules..." << std::endl;
	VkShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.codeSize = shaderCode.size();
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice_, &shaderInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	std::cout << "Shader Module Created" << std::endl;

	return shaderModule;
}

//------------------------------------------------------------------------------------------------------

VkSampleCountFlagBits VulkanCore::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties = physicalDeviceProperties_.getVkPhysicalDeviceProperties();

    VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    if(counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;
    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;
    if (counts &  VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;
    if (counts &  VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;
    if (counts &  VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;


    return VK_SAMPLE_COUNT_1_BIT;
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::recreateSwapChain()
{
	vkDeviceWaitIdle(logicalDevice_);

	cleanUpSwapChain();
	createSwapChain();
	createRenderPass();
	createGraphicsPipeline();
    createDepthRessources();
    createColorRessources();
    swapchain_.createFramebuffers(renderPass_, {colorImageView_, depthImageView_});
	createCommandBuffers();
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::cleanUpSwapChain()
{
	swapchain_.destroyFramebuffers();

	vkFreeCommandBuffers(logicalDevice_, commandPool_, (uint32_t)commandBuffers_.size(), commandBuffers_.data());
	
    vkDestroyImageView(logicalDevice_, depthImageView_, nullptr);
    vkDestroyImage(logicalDevice_, depthImage_, nullptr);
    vkFreeMemory(logicalDevice_, depthImageMemory_, nullptr);

    vkDestroyImageView(logicalDevice_, colorImageView_, nullptr);
    vkDestroyImage(logicalDevice_, colorImage_, nullptr);
    vkFreeMemory(logicalDevice_, colorMemory_, nullptr);

	vkDestroyPipeline(logicalDevice_, graphicsPipeline_, nullptr);
	vkDestroyPipelineLayout(logicalDevice_, pipelineLayout_, nullptr);
	vkDestroyRenderPass(logicalDevice_, renderPass_, nullptr);
	swapchain_.destroy();
}

//------------------------------------------------------------------------------------------------------

void VulkanCore::cleanup()
{
    if(!isCleaned_)
    {
        isCleaned_ = true;
        cleanUpSwapChain();

        //Descriptor Set/Pool
        vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
        vkDestroyDescriptorSetLayout(logicalDevice_, descriptorSetLayout_, nullptr);

        lenaTexture_.destroy();

        //Vertex/Uniform/Index buffers
        for (size_t i = 0; i < swapchain_.getImages().size(); i++)
        {
            vkDestroyBuffer(logicalDevice_, uniformBuffers_[i], nullptr);
            vkFreeMemory(logicalDevice_, uniformBuffersMemory_[i], nullptr);
        }

        model_.destroy();


        //Semaphores
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(logicalDevice_, imageAvailableSemaphore_[i], nullptr);
            vkDestroySemaphore(logicalDevice_, renderFinishedSemaphore_[i], nullptr);
            vkDestroyFence(logicalDevice_, inFlightFences_[i], nullptr);
        }

        //Command Pool
        vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
        if (physicalDeviceProperties_.getQueueFamilyIndices().transferAvailable())
        {
            vkDestroyCommandPool(logicalDevice_, commandPoolTransfert_, nullptr);
        }


        vkDestroyDevice(logicalDevice_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        if (ENABLE_VALIDATION_LAYERS)
        {
            debugMessenger_.destroy();
        }
        vkDestroyInstance(instance_, nullptr);
    }
}

}
