#include "VkMesh.h"

VkMesh::VkMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex> *vertices) {
    vertexCount = vertices->size();
    physicalDevice = newPhysicalDevice;
    device = newDevice;
    createVertexBuffer(vertices);
}

void
VkMesh::createVertexBuffer(std::vector<Vertex> *vertices) {

    // Information to describe the buffer
    VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(Vertex) * vertices->size(),           // Size of buffer (size of 1 vertex * number of vertices)
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,        // TODO: See VkBufferUsageFlagBits for more usages (types of buffer)
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE      // Sharing mode of using vertex buffers
    };

    VkResult result = vkCreateBuffer(device, &bufferCreateInfo, VK_ALLOCATOR, &vertexBuffer);

    // Manage memory requires allocator

    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Vertex buffer!");
    }

    // Get buffer memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

    // Allocate memory to VkDevice
    VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memoryRequirements.size,          // How much we want to allocate in the buffer
            .memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };

    result = vkAllocateMemory(device, &memoryAllocateInfo, VK_ALLOCATOR, &vertexBufferMemory);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Vertex buffer memory");
    }

    // Bind memory

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    // Map memory to vertex buffer

    void* data;                                                                     // Create pointer to a point in normal memory
    vkMapMemory(device, vertexBufferMemory,                                 // Map the vertex buffer memory to that point
                0, bufferCreateInfo.size,
                0, &data);

    memcpy(data, vertices->data(), (size_t) bufferCreateInfo.size);      // Copy memory from vertices to that point
    vkUnmapMemory(device, vertexBufferMemory);                              // Unmap vertex buffer memory



}

void
VkMesh::destroyVertexBuffer() {

    vkDestroyBuffer(device, vertexBuffer, VK_ALLOCATOR);
    vkFreeMemory(device, vertexBufferMemory, VK_ALLOCATOR);
}

uint32_t
VkMesh::findMemoryTypeIndex(uint32_t allowedTypes, int properties) {
    // Get Properties of physical device memory
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice,&memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {

        if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties ) {
            return i;
        }
    }
    return -1;
}

VkMesh::VkMesh() {

}
VkMesh::~VkMesh() {

}

