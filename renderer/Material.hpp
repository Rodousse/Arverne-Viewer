#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "renderer/VkElement.hpp"
#include "renderer/texture/MaterialTexture.hpp"

namespace renderer{

class Material : VkElement
{
protected:
    MaterialTexture albedo;

public:
    Material(const VulkanCore *pCore);

    virtual void create() override;
    virtual void destroy() override;

    void setAlbedo(const MaterialTexture& texture);

    void getDescriptorSet();

    virtual ~Material() override;

};


}
#endif // MATERIAL_HPP
