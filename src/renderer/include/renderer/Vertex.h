#pragma once

#include <data/3D/VertexAttribute.h>
#include <vulkan/vulkan.h>
#include <array>


namespace renderer
{

struct Vertex : public data::VertexAttribute
{
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

}
