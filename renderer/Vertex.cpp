#include "Vertex.hpp"


VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Also could be VK_VERTEX_INPUT_RATE_INSTANCE for instance rendering 

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attribDescriptions = {};
	attribDescriptions[0].binding = 0;
    attribDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribDescriptions[0].location = 0;
	attribDescriptions[0].offset = offsetof(Vertex, pos);

	attribDescriptions[1].binding = 0;
	attribDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribDescriptions[1].location = 1;
	attribDescriptions[1].offset = offsetof(Vertex, color);

	attribDescriptions[2].binding = 0;
	attribDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attribDescriptions[2].location = 2;
	attribDescriptions[2].offset = offsetof(Vertex, texCoord);

	return attribDescriptions;


}
