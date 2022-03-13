#pragma once
#include <cstdint>
#include <vector>

const std::vector<const char *> deviceExtensions ={
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices of queues families if they exist at all
struct
QueueFamilyIndexes {
    uint32_t graphicsFamilyIndex = -1;        // Location of graphics queue family
    uint32_t presentationFamilyIndex  = -1;

    // check if queue families are valid

    [[nodiscard]] bool isValid() const {
        return graphicsFamilyIndex >= 0 && presentationFamilyIndex >= 0;
    }
};

struct
SwapChainDetails {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;     // Image size, extents, etc.
    std::vector<VkSurfaceFormatKHR> surfaceFormatKhr;    // Surface Image format, eg. RGBA and size of each color
    std::vector<VkPresentModeKHR> presentModeKhr;        // How images should be presented in the screen
};