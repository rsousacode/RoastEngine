//
// Created by miracs91 on 12.03.2022.
//

#include <stdexcept>
#include <vector>
#include "vkRenderer.h"

// Initialization of the Renderer.
int
vkRenderer::init(GLFWwindow *newWindow) {

    w = newWindow;

    try {
        createInstance();
        createPhysicalDevices();
        createLogicalDevice();
    }
    catch (const std::runtime_error &e) {
        printf("ERROR %s\n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}

// Create Vulkan Instance
void
vkRenderer::createInstance() {
    VkApplicationInfo appInfo = {
            .sType                   = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName        = "RoastEngine",
            .applicationVersion      = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName             = "NoEngine",
            .engineVersion           = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion              = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo createInfo = {
            .sType                  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo       = &appInfo
    };

    // Create list to hold instance extensions
    std::vector<const char *> vkExtensions = std::vector<const char *>();

    const char **glfwExtensions;
    uint32_t glfwExtensionCount = 0;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (size_t i = 0; i < glfwExtensionCount; ++i) {
        vkExtensions.push_back(glfwExtensions[i]);
    }

    // Check instance extensions support
    if (!checkInstanceExtensionSupport(&vkExtensions)) {
        throw std::runtime_error("There's no extension supported from this platform.");
    }

    createInfo.enabledExtensionCount    = static_cast<uint32_t>(vkExtensions.size());
    createInfo.ppEnabledExtensionNames  = vkExtensions.data();

    // Validation Layers
    createInfo.enabledLayerCount        = 0;
    createInfo.ppEnabledLayerNames      = nullptr;

    // Create instance
    VkResult r                          = vkCreateInstance(&createInfo, nullptr, &instance);

    if (r != VK_SUCCESS) {
        throw std::runtime_error("Error creating a Vk instance.");
    }
}

vkRenderer::vkRenderer() {

}

vkRenderer::~vkRenderer() {

}

// Check support for vulkan extensions.
bool
vkRenderer::checkInstanceExtensionSupport(std::vector<const char *> *checkExtensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // check if given extensions are in list of available extensions
    for (const auto &checkExtension: *checkExtensions) {

        bool hasExtension = extensionInList(extensions, checkExtension);

        if (!hasExtension) return false;
    }

    return true;
}

// Check if c_string (extension) is in list
bool
vkRenderer::extensionInList(std::vector<VkExtensionProperties> &extensions, const char *const &checkExtension) const {
    bool hasExtension = false;
    for (const auto &extension: extensions) {
        if (strcmp(checkExtension, extension.extensionName) == 0) {
            hasExtension = true;
            break;
        }
    }
    return hasExtension;
}

// Destroy Vulkan instance
void
vkRenderer::cleanup() {
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

// Enumerate Physical Devices
void
vkRenderer::createPhysicalDevices() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    // If no devices available, none support Vulkan
    if (deviceCount == 0) {
        throw std::runtime_error("Can't find GPUs that support Vulkan instance.");
    }

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    // Get actual physical device
    for(const auto &device : deviceList) {
        if(deviceIsSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

// Check if device is suitable to run Vulkan
bool
vkRenderer::deviceIsSuitable(VkPhysicalDevice device) {

    /*
    // Info about the device (ID, name, type, vendor, etc)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // Information about what the device can do (geo shader, tess shader, wide lines, etc)

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
     */

    QueueFamilyIndexes indexes = getQueueFamilies(device);
    return indexes.graphicsFamilyIndex >= 0;
}

// Get all Queue Family Property info for the given device
QueueFamilyIndexes
vkRenderer::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndexes indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    // Go through each queue family and check if it has at least 1 of the required types of queue
    // First check if queue family has at least 1 queue in that family (could have no queues)
    // Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to
    // check if it has required type.
    uint32_t i = 0;
    for(const auto &queueFamily : queueFamilyList) {
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamilyIndex = i;
            break;
        }
        i++;

    }
    return  indices;
}

void
vkRenderer::createLogicalDevice() {

    QueueFamilyIndexes indices = getQueueFamilies(mainDevice.physicalDevice);


    float queuePriority = 1.;
    // Queue the logical device needs to create and info to do so
    VkDeviceQueueCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.graphicsFamilyIndex,
            .queueCount = 1 ,                                       // Number of queues to create
            .pQueuePriorities = &queuePriority                   // QueueFamily priority (1 = highest priority)

    };

    // Physical Device Features the Logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures {};

    // Information to create logical device
    VkDeviceCreateInfo deviceCreateInfo = {
            .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount       = 1,              // Number of Queue Create Infos
            .pQueueCreateInfos          = &createInfo,      // List of queue create infos
            .enabledLayerCount          = 0,                // Validation layers (deprecated in VK API 1.1+)
            .enabledExtensionCount      = 0,             // Logical Device extensions
            .ppEnabledExtensionNames    = nullptr,    // List of enabled logical device extensions
            .pEnabledFeatures           = &deviceFeatures    // Physical Device Features the Logical device will be using
    };

    // Create the logical device for the given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice,
                                     &deviceCreateInfo,
                                     nullptr,
                                     &mainDevice.logicalDevice);

    if( result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Logical Device!");
    }

    // from given logical device of given queue family of given queue index (0 since only one queue)
    // assign Queue reference in given graphicsQueue
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamilyIndex, 0, &graphicsQueue);
}

bool vkRenderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check if all of the layers in validationLayers exist in available layers

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if(!layerFound) return false;
    }
    return true;
}
