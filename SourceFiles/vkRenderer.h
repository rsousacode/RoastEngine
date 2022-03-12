//
// Created by miracs91 on 12.03.2022.
//

#ifndef ROASTENGINE_VKRENDERER_H
#define ROASTENGINE_VKRENDERER_H

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.h"
#include "Utilities.h"

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

    void refreshPhysicalDevices();



    // Extension Support
    bool checkInstanceExtensionSupport  (std::vector<const char*> *checkExtensions);
    bool extensionInList                (std::vector<VkExtensionProperties> &extensions, const char *const &checkExtension) const;

    // Device Support
    bool deviceIsSuitable(VkPhysicalDevice device);

    QueueFamilyIndexes getQueueFamilies(VkPhysicalDevice device);

    void createLogicalDevice();
};


#endif //ROASTENGINE_VKRENDERER_H
