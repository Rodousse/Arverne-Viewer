#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include <array>


namespace renderer
{

struct Vertex
{
    glm::vec3 pos;
    //QVector3D color;
    glm::vec3 normal;
    glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    bool operator==(const Vertex& other) const;
};

}

#endif
