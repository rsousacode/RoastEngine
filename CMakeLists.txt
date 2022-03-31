cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
project(RoastEngine)

# Use C++ version 23
set(CMAKE_CXX_STANDARD 23)

# Library content for Vulkan and other libraries
link_directories(/usr/local/lib)
link_libraries(/usr/local/lib)

# Source Files
add_executable(RoastEngine
        src/main.cpp
        src/vk/VkRender.cpp
        src/vk/VkRender.h
        src/RoastDisplay.mm
        src/RoastDisplay.h
        src/ogl/LoadShaders.h
        src/ogl/LoadShaders.cpp

        src/ogl/vgl.h

        src/ogl/OglRender.cpp
        src/ogl/OglRender.h

        src/mtl/MtlRender.mm
        src/mtl/MtlRender.h
        src/RDDefs.h
        src/common/RDDebug.h
        src/RMath.h)


# Engine shader compilation step
add_custom_target(SPV_SHADER_COMP
        COMMAND sh src/vk/comp_rshader.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

find_program(RD_SHELL_PATH sh)

if(RD_SHELL_PATH)
    message(STATUS "+ Unix Shell: ${RD_SHELL_PATH}")
    add_dependencies(RoastEngine SPV_SHADER_COMP)
endif(RD_SHELL_PATH)
# End of Engine shader compilation step

# Library Includes for Vulkan and other libraries
include_directories(deps/glfw/include)

# GLFW Includes
include_directories(/usr/local/include/)

# GL3W Includes
include_directories(deps/gl3w/include)

# GLM mathematical functions
add_subdirectory(deps/glm/)
add_subdirectory(deps/glfw/)

# Vulkan and GLFW3 for window creation
find_package(Vulkan)
find_package(glfw3)
#add_subdirectory(deps/imgui)

# Target dependencies
target_link_libraries(RoastEngine Vulkan)
target_link_libraries(RoastEngine glm)
#target_link_libraries(RoastEngine imgui)
target_link_libraries(RoastEngine glfw)
target_link_libraries(RoastEngine "-framework OpenGL")
target_link_libraries(RoastEngine "-framework QuartzCore")
target_link_libraries(RoastEngine "-framework Metal")
target_link_libraries(RoastEngine "-framework AppKit")
target_link_libraries(RoastEngine "-framework COCOA")
target_link_libraries(RoastEngine "-framework IOKit")
target_link_libraries(RoastEngine "-framework CoreVideo")




