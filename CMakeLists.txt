set(CLANG_ENABLE_OBJC_WEAK, )

cmake_minimum_required(VERSION 3.21)
project(RoastEngine)

# Use C++ version 23
set(CMAKE_CXX_STANDARD 23)

# Library content for Vulkan and other libraries
link_libraries(/usr/local/lib)
link_directories(/usr/local/lib)

include_directories(deps/imgui)
include_directories(deps/imgui/backends)

file( GLOB IMGUI_SRC
        deps/imgui/*.h
        deps/imgui/*.cpp
        deps/imgui/backends/imgui_impl_metal.*
        deps/imgui/backends/imgui_impl_glfw.*
        deps/imgui/backends/imgui_impl_opengl3.*
        )

# Source Files
add_executable(RoastEngine
        src/main.cpp
        src/vk/VkRender.cpp
        src/vk/VkRender.h
        ${IMGUI_SRC}
        src/_roast/RoastDisplay.mm
        src/_roast/RoastDisplay.h
        src/ogl/OglRender.cpp
        src/ogl/OglRender.h
        src/mtl/MtlRender.mm
        src/mtl/MtlRender.h
        src/_roast/RDDefs.h
        deps/imgui/imgui.h
)


# Engine shader compilation step
add_custom_target(SPV_SHADER_COMP
        COMMAND sh comp_rshader.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

find_program(RD_SHELL_PATH sh)

if(RD_SHELL_PATH)
    message(STATUS "+ Unix Shell: ${RD_SHELL_PATH}")
    add_dependencies(RoastEngine SPV_SHADER_COMP)
endif(RD_SHELL_PATH)
# End of Engine shader compilation step

# Library Includes for Vulkan and other libraries
include_directories(/usr/local/include/)

# GLM mathematical functions
add_subdirectory(deps/glm/)

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




