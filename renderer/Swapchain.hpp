#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "VkElement.hpp"
#include <iostream>
#include <vector>

namespace renderer
{


class Swapchain :
	public VkElement
{
private:
	using VkElement::pCore_;

	VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
	std::vector<VkImage> images_;
	std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;

	VkSurfaceFormatKHR format_;
	VkExtent2D extent_;
	VkExtent2D redimensionnedExtent_;
    VkPresentModeKHR presentMode_;

	void createSwapchain();
    void createImageViews();

	void chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
    void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	
public:
	Swapchain(const VulkanCore *pCore);
	~Swapchain();

	void create();
	void destroy();
	void setExtent(const VkExtent2D& extent);
    void createFramebuffers(const VkRenderPass& pRenderPass, const std::vector<VkImageView>& attachements);
	void destroyFramebuffers();
	
	const VkSwapchainKHR& getVkSwapchain()const;
	const std::vector<VkImage>& getImages()const;
	const std::vector<VkImageView>& getImageViews()const;
	const std::vector<VkFramebuffer>& getFramebuffers()const;

	const VkSurfaceFormatKHR& getFormat()const;
	const VkExtent2D& getExtent()const;
	const VkPresentModeKHR& getPresentMode()const;


};


}

#endif
