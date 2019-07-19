#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <array>


namespace data
{

struct VertexAttribute
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const VertexAttribute& other) const;
};

}
