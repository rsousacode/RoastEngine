//
// Created by miracs91 on 12.03.2022.
//

#ifndef ROASTENGINE_VKRENDERER_H
#define ROASTENGINE_VKRENDERER_H

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.h"
#include "Utilities.h"
#include <vector>

class vkRenderer {

public:
    vkRenderer();

    int init(GLFWwindow *newWindow);
    void cleanup();

    ~vkRenderer();


private:
    GLFWwindow *w;

    // vk components
    VkInstance instance;

    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    VkQueue graphicsQueue;

    // vk functions
    void createInstance();
    void createPhysicalDevices();
    void createLogicalDevice();

    // Extension Support
    bool checkInstanceExtensionSupport  (std::vector<const char*> *checkExtensions);
    bool extensionInList                (std::vector<VkExtensionProperties> &extensions, const char *const &checkExtension) const;

    // Device Support
    bool deviceIsSuitable(VkPhysicalDevice device);
    QueueFamilyIndexes getQueueFamilies(VkPhysicalDevice device);


    // Validation layers
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    bool checkValidationLayerSupport();
};


#endif //ROASTENGINE_VKRENDERER_H
