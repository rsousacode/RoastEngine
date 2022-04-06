#ifndef ROASTENGINE_VKMESH_H
#define ROASTENGINE_VKMESH_H
#define GLFW_INCLUDE_VULKAN
#import <GLFW/glfw3.h>
#import <vector>
#import "utils.h"

#define VK_ALLOCATOR nullptr

#endif //ROASTENGINE_VKMESH_H


struct VkMesh {

    VkMesh();
    ~VkMesh();
    VkMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex> *vertices);
    int vertexCount;
    VkPhysicalDevice physicalDevice;

    VkDevice device;

    VkDeviceMemory deviceMemory;

    void createVertexBuffer(std::vector<Vertex> *vertices);

    void destroyVertexBuffer();

    uint32_t  findMemoryTypeIndex(uint32_t allowedTypes, // Types the buffer is compatible with
                                  int properties); // Compatible types

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};