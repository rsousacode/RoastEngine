cmake_minimum_required(VERSION 3.21)

project(RoastEngine)

set(CMAKE_CXX_STANDARD 23)

link_directories(/usr/local/lib)

add_executable(RoastEngine SourceFiles/main.cpp SourceFiles/vkRenderer.cpp SourceFiles/vkRenderer.h)

include_directories(/Users/miracs91/VulkanSDK/1.3.204.1/macOS/include)

include_directories(/Users/miracs91/VulkanSDK/1.3.204.1/macOS/)

find_package(Vulkan)

add_subdirectory(deps/glm/)

find_package(glfw3)

target_link_libraries(RoastEngine Vulkan)
target_link_libraries(RoastEngine glm)
target_link_libraries(RoastEngine glfw)




