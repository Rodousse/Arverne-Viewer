#include "renderer/VulkanApplication.h"

namespace renderer
{

VulkanApplication::VulkanApplication()
{
}


VulkanApplication::~VulkanApplication()
{
}

void VulkanApplication::setName(const std::string& name)
{
    name_.assign(name);
}

const std::string& VulkanApplication::getName()const
{
    return name_;
}

}
