cmake_minimum_required(VERSION 3.10)


set(PROJECT_NAME "ArverneViewer")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME})

#CMake config

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/config/cmake/")

# third party dependency

find_package(Vulkan REQUIRED)
find_package(TinyObjLoader REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)
find_package(plog REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)


#Set the output directory for the binaries

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")

# Add The configure file here : https://stackoverflow.com/questions/8887117/can-cmake-generate-configure-file
set(RESOURCE_PATH ${PROJECT_SOURCE_DIR}/resources)
configure_file(config/cmake/defines.h.in include/defines.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include/)



# Project subdirectories
add_subdirectory(src/data)
add_subdirectory(src/loader)
add_subdirectory(src/renderer)
add_subdirectory(src/application)



