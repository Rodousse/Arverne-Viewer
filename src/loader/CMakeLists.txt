

set(HEADERS
    include/loader/ImageLoader.hpp
    include/loader/ObjLoader.h
    src/Loader.h
)

set(SOURCES
    src/Loader.cpp
    src/ObjLoader.cpp
)

add_library(loader STATIC ${SOURCES} ${HEADERS})
target_link_libraries(loader data)
target_include_directories(loader
PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${TINYOBJLOADER_INCLUDE_DIR}
    ${PLOG_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
PRIVATE
    src/)

