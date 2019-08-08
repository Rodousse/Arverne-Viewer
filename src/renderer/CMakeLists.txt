

set(HEADERS
    include/renderer/camera/ArcBallCamera.h
    include/renderer/camera/Camera.h
    include/renderer/texture/MaterialTexture.h
    include/renderer/texture/Texture2D.h
    include/renderer/tools/ImageTools.h
    include/renderer/tools/MemoryTools.h
    include/renderer/tools/VulkanTools.h
    include/renderer/DebugMessenger.h
    include/renderer/Material.h
    include/renderer/Model.h
    include/renderer/PhysicalDeviceProperties.h
    include/renderer/PhysicalDeviceProvider.h
    include/renderer/Swapchain.h
    include/renderer/Vertex.h
    include/renderer/VkElement.h
    include/renderer/VulkanApplication.h
    include/renderer/VulkanCore.h
)

set(SOURCES
    src/camera/ArcBallCamera.cpp
    src/camera/Camera.cpp
    src/texture/MaterialTexture.cpp
    src/texture/Texture2D.cpp
    src/tools/ImageTools.cpp
    src/tools/MemoryTools.cpp
    src/tools/VulkanTools.cpp
    src/DebugMessenger.cpp
    src/Material.cpp
    src/Model.cpp
    src/PhysicalDeviceProperties.cpp
    src/PhysicalDeviceProvider.cpp
    src/Swapchain.cpp
    src/Vertex.cpp
    src/VkElement.cpp
    src/VulkanApplication.cpp
    src/VulkanCore.cpp
)

set(SHADER_FILES
${CMAKE_SOURCE_DIR}/resources/shaders/vertex.vert
${CMAKE_SOURCE_DIR}/resources/shaders/fragment.frag
# ${CMAKE_SOURCE_DIR}/resources/shaders/CubeMap.vert
# ${CMAKE_SOURCE_DIR}/resources/shaders/CubeMap.frag
)

set(SHADER_PATH ${RESOURCE_PATH}/shaders)

add_custom_target(
compileShaders
DEPENDS ${SHADER_FILES}
COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator -o ${SHADER_PATH}/vert.spv -V ${SHADER_PATH}/vertex.vert
COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator -o ${SHADER_PATH}/frag.spv -V ${SHADER_PATH}/fragment.frag
# COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator -o ${SHADER_PATH}/cubemapVert.spv -V ${SHADER_PATH}/CubeMap.vert
# COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator -o ${SHADER_PATH}/cubemapFrag.spv -V ${SHADER_PATH}/CubeMap.frag
)

add_library(renderer STATIC ${SOURCES} ${HEADERS} ${SHADER_FILES})
add_dependencies(renderer compileShaders)
target_link_libraries(renderer Vulkan::Vulkan loader data)
target_include_directories(renderer PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include ${STB_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})

