#ifndef MATERIALTEXTURE_HPP
#define MATERIALTEXTURE_HPP

#include "Texture2D.hpp"


namespace renderer{

class MaterialTexture : public Texture2D
{
protected:

    std::string path_;
    VkSampler sampler_;
    uint32_t mipLevels_ = 1;


    virtual void createImage()override;
    virtual void createSampler();
    virtual void createImageView()override;
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

public:
    MaterialTexture(const VulkanCore* pCore);
    MaterialTexture(const VulkanCore* pCore, const VkFormat& format);
    MaterialTexture(const VulkanCore* pCore, const std::string& path, const VkFormat& format);

    virtual void create() override;
    virtual void destroy() override;

    const VkSampler& getSampler()const;


    void setFilePath(const std::string& path);
};

}

#endif // MATERIALTEXTURE_HPP
