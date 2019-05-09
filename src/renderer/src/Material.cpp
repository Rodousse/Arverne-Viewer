#include "renderer/Material.h"
namespace renderer
{

Material::Material(const VulkanCore* pCore):
    VkElement(pCore),
    albedo(pCore)
{

}

void Material::create()
{
    isCreated_ = true;
}

void Material::destroy()
{
    isCreated_ = false;
}

Material::~Material()
{

}


}
