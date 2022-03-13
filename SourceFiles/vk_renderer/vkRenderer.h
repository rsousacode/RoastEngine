//
// Created by miracs91 on 12.03.2022.
//

#ifndef ROASTENGINE_VKRENDERER_H
#define ROASTENGINE_VKRENDERER_H
#define GLFW_INCLUDE_VULKAN
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "../utils.h"
#include <vector>

class vkRenderer {

public:
    vkRenderer();

    int init(GLFWwindow *newWindow);
    void cleanup();

    ~vkRenderer();


private:
    GLFWwindow *w{};

    // Vulkan Instance
    VkInstance instance{};

    // Debug Implementation (Validation Layers)
    VkDebugUtilsMessengerEXT debugMessenger{};

    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice{};

    VkQueue graphicsQueue{};
    VkQueue presentationQueue{};
    VkSurfaceKHR surface{};

    // Instance Support
    void createInstance();

    // Devices Support
    void createPhysicalDevices();
    void createLogicalDevice();
    void createSurface();

    // Extension Support
    bool checkInstanceExtensionSupport  (std::vector<const char*> *checkExtensions);
    bool extensionInList                (std::vector<VkExtensionProperties> &extensions, const char *const &checkExtension) const;

    // Device Support
    bool deviceIsSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndexes getQueueFamilies(VkPhysicalDevice device);


    // Validation layers
    // TODO: creation and clean handling

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };


    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void setupDebugMessenger();
};

#endif //ROASTENGINE_VKRENDERER_H
