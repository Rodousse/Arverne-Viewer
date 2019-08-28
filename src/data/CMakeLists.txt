

set(HEADERS
    include/data/3D/Mesh.h
    include/data/3D/VertexAttribute.h
    include/data/2D/Image.hpp
)

set(SOURCES
    src/3D/Mesh.cpp
    src/3D/VertexAttribute.cpp
)

add_library(data STATIC ${SOURCES} ${HEADERS})
target_include_directories(data
PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${GLM_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
)

