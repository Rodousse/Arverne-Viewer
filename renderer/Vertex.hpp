#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <QVector3D>
#include <QVector2D>
#include <vulkan/vulkan.h>
#include <array>


struct Vertex
{
    QVector3D pos;
    QVector3D color;
    QVector2D texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};


#endif
