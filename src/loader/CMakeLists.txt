

set(HEADERS
    include/loader/ImageLoader.h
    include/loader/Loader.h
    include/loader/ObjLoader.h
)

set(SOURCES
    src/ImageLoader.cpp
    src/Loader.cpp
    src/ObjLoader.cpp
)

add_library(loader STATIC ${SOURCES} ${HEADERS})
target_link_libraries(loader data)
target_include_directories(loader PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include ${TINYOBJLOADER_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})

