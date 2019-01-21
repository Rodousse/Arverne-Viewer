#include "Swapchain.hpp"
#include "VulkanCore.hpp"
#include <algorithm>


Swapchain::Swapchain(const VulkanCore *pCore):
	VkElement(pCore)
{
}

//------------------------------------------------------------------------------------------------------

Swapchain::~Swapchain()
{
}

//------------------------------------------------------------------------------------------------------

void Swapchain::create()
{
	createSwapchain();
    createImageViews();
    isCreated_ = true;
}

//------------------------------------------------------------------------------------------------------

void Swapchain::destroy()
{
	for (auto imageView : imageViews_)
	{
		vkDestroyImageView(pCore_->getDevice(), imageView, nullptr);
	}
	vkDestroySwapchainKHR(pCore_->getDevice(), swapchain_, nullptr);
}

//------------------------------------------------------------------------------------------------------

void Swapchain::setExtent(const VkExtent2D & extent)
{
	redimensionnedExtent_ = extent;
}

//------------------------------------------------------------------------------------------------------

void Swapchain::createFramebuffers(const VkRenderPass& renderPass, const std::vector<VkImageView>& attachements)
{

	std::cout << "Creating Framebuffers..." << std::endl;
	framebuffers_.resize(imageViews_.size());
    std::vector<VkImageView> frameAttachements;
    frameAttachements.insert(frameAttachements.begin(), attachements.begin(), attachements.end());
	for (size_t i = 0; i < imageViews_.size(); i++)
	{

        frameAttachements.push_back(imageViews_[i]);
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(frameAttachements.size());
        framebufferInfo.pAttachments = frameAttachements.data();
		framebufferInfo.layers = 1;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.height = extent_.height;
		framebufferInfo.width = extent_.width;

		if (vkCreateFramebuffer(pCore_->getDevice(), &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffers!");
		}
        frameAttachements.pop_back();
	}

	std::cout << "Framebuffers Created" << std::endl;
}

void Swapchain::destroyFramebuffers()
{
	for (auto framebuffer : framebuffers_)
	{
		vkDestroyFramebuffer(pCore_->getDevice(), framebuffer, nullptr);
	}
}

//------------------------------------------------------------------------------------------------------

const VkSwapchainKHR & Swapchain::getVkSwapchain()const
{
	return swapchain_;
}

//------------------------------------------------------------------------------------------------------

const std::vector<VkImage>& Swapchain::getImages()const
{
	return images_;
}

//------------------------------------------------------------------------------------------------------

const std::vector<VkImageView>& Swapchain::getImageViews()const
{
	return imageViews_;
}

const std::vector<VkFramebuffer>& Swapchain::getFramebuffers()const
{
	return framebuffers_;
}

//------------------------------------------------------------------------------------------------------

const VkSurfaceFormatKHR & Swapchain::getFormat()const
{
	return format_;
}

//------------------------------------------------------------------------------------------------------

const VkExtent2D & Swapchain::getExtent()const
{
	return extent_;
}

//------------------------------------------------------------------------------------------------------

const VkPresentModeKHR & Swapchain::getPresentMode()const
{
	return presentMode_;
}

//------------------------------------------------------------------------------------------------------

void Swapchain::createSwapchain()
{
	SwapChainSupportDetails swapChainSupport = pCore_->getPhysicalDeviceProperties().getSwapChainSupportDetails();

	chooseSwapSurfaceFormat(swapChainSupport.surfaceFormats);
	chooseSwapExtent(swapChainSupport.surfaceCapabilities);
	chooseSwapPresentMode(swapChainSupport.presentModes);

	uint32_t imageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1;
	if (swapChainSupport.surfaceCapabilities.maxImageCount > 0
		&& imageCount > swapChainSupport.surfaceCapabilities.maxImageCount)
	{
		imageCount = swapChainSupport.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainInfo = {};
	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.surface = pCore_->getSurface();
	swapChainInfo.imageExtent = extent_;
	swapChainInfo.imageFormat = format_.format;
	swapChainInfo.imageColorSpace = format_.colorSpace;
	swapChainInfo.presentMode = presentMode_;
	swapChainInfo.minImageCount = imageCount;
	swapChainInfo.imageArrayLayers = 1; // Number of layers in the image (different in 3d stereoscopic)
	swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	QueueFamilyIndices indices = pCore_->getPhysicalDeviceProperties().getQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = {
		static_cast<uint32_t>(indices.graphicsFamily),
		static_cast<uint32_t>(indices.presentingFamily)
	};

	if (indices.graphicsFamily != indices.presentingFamily)
	{
		//This line provides us the benefit to share an image in the swapchain across all queue family
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainInfo.queueFamilyIndexCount = 2;
		swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainInfo.queueFamilyIndexCount = 0;
		swapChainInfo.pQueueFamilyIndices = nullptr;
	}

	swapChainInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
	swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.clipped = VK_TRUE;
	swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(pCore_->getDevice(), &swapChainInfo, nullptr, &swapchain_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(pCore_->getDevice(), swapchain_, &imageCount, nullptr);
	images_.resize(imageCount);
	vkGetSwapchainImagesKHR(pCore_->getDevice(), swapchain_, &imageCount, images_.data());
}

//------------------------------------------------------------------------------------------------------

void Swapchain::createImageViews()
{
	imageViews_.resize(images_.size());

	//Configure image view for every image in the swapchain
	for (size_t i = 0; i < images_.size(); i++)
	{
        imageViews_[i] = pCore_->getUtils().createImageView(format_.format, images_[i], VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

}

//------------------------------------------------------------------------------------------------------

/*@brief : Choose the optimal surface format for the swap chain and return it
*/
void Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		format_ = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		return;
	}

	for (const auto& format : availableFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			format_ = format;
			return;
		}
	}
	format_ = availableFormats[0];
}

//------------------------------------------------------------------------------------------------------

/*@brief : Choose the optimal present mode for the swap chain and return it
*/
void Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	VkPresentModeKHR bestmode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& mode : availableModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			presentMode_ = mode;
			return;
		}
		else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestmode = mode;
		}
	}
	presentMode_ = bestmode;
}

//------------------------------------------------------------------------------------------------------

void Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		extent_ = capabilities.currentExtent;
		return;
	}
	else
	{
		VkExtent2D actualExtent = redimensionnedExtent_;
		actualExtent.width =
			std::max(capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width,
					actualExtent.width));
		actualExtent.height =
			std::max(capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height,
					actualExtent.height));

		extent_ = actualExtent;
	}
}

