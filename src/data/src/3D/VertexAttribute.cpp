#include "data/3D/VertexAttribute.h"


namespace data
{

bool VertexAttribute::operator==(const VertexAttribute& other) const
{
    return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
}

}
