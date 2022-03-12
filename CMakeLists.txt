cmake_minimum_required(VERSION 3.21)
project(RoastEngine)

# Use C++ version 23
set(CMAKE_CXX_STANDARD 23)

# Library content for Vulkan and other libraries
link_directories(/usr/local/lib)

# Source Files
add_executable(RoastEngine
        SourceFiles/main.cpp
        SourceFiles/vkRenderer.cpp
        SourceFiles/vkRenderer.h)

# Library Includes for Vulkan and other libraries
include_directories(/usr/local/include/)

# GLM mathematical functions
add_subdirectory(deps/glm/)

# Vulkan and GLFW3 for window creation
find_package(Vulkan)
find_package(glfw3)

# Target dependencies 
target_link_libraries(RoastEngine Vulkan)
target_link_libraries(RoastEngine glm)
target_link_libraries(RoastEngine glfw)




