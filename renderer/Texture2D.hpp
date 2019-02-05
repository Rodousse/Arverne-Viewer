#ifndef TEXTURE_2D_HPP
#define TEXTURE_2D_HPP

#include "VkElement.hpp"
#include <string>
#include <QImage>

namespace renderer
{

class Texture2D :
	public VkElement
{
protected:
	using VkElement::pCore_;
	std::string path_;
	std::string name_;

	VkImage image_;
	VkImageView imageView_;
	VkDeviceMemory imageMemory_;
	VkSampler sampler_;

    VkFormat format_;
    uint32_t mipLevels_ = 1;

	void createImage();
	void createImageView();
    void createSampler();
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

public:
	Texture2D(const VulkanCore* pCore);
    Texture2D(const VulkanCore* pCore, const std::string& path, const VkFormat& format);
    void create() override;
    void destroy() override;
	
	void setFilePath(const std::string& path);
	const VkImage& getImage()const;
	const VkImageView& getImageView()const;
	const VkSampler& getSampler()const;

    ~Texture2D() override;
};

}

#endif
