cmake_minimum_required(VERSION 3.21)
project(RoastEngine)

# Use C++ version 23
set(CMAKE_CXX_STANDARD 23)

# Library content for Vulkan and other libraries
link_directories(/usr/local/lib)
link_libraries(/usr/local/lib)

# Source Files
add_executable(RoastEngine
        src/main.cpp
        src/vk/vkRenderer.cpp
        src/vk/vkRenderer.h
        src/_roast/RoastDisplay.mm
        src/_roast/RoastDisplay.h
        src/ogl/oglRenderer.cpp
        src/ogl/oglRenderer.h
        src/mtl/mtlRenderer.mm
        src/mtl/mtlRenderer.h)

# Library Includes for Vulkan and other libraries
include_directories(/usr/local/include/)

#GLEW
include_directories(/opt/homebrew/Cellar/glew/2.2.0_1/include)
include_directories(/opt/homebrew/Cellar/glew/2.2.0_1/lib)
include_directories(/opt/homebrew/Cellar/glew/2.2.0_1)

# GLM mathematical functions
add_subdirectory(deps/glm/)

# Vulkan and GLFW3 for window creation
find_package(Vulkan)

find_package(glfw3)
find_package(OpenGL REQUIRED)
find_package(GLEW)

# Target dependencies

target_link_libraries(RoastEngine Vulkan)

target_link_libraries(RoastEngine glm)
#target_link_libraries(RoastEngine "-framework glew")
#target_link_libraries(RoastEngine glew)

target_link_libraries(RoastEngine glfw)

target_link_libraries(RoastEngine "-framework OpenGL")

target_link_libraries(RoastEngine "-framework QuartzCore")
target_link_libraries(RoastEngine "-framework Metal")
target_link_libraries(RoastEngine "-framework AppKit")




