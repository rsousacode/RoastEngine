//
// Created by miracs91 on 12.03.2022.
//

#include <stdexcept>
#include <vector>
#include <iostream>
#include "vkRenderer.h"



vkRenderer::vkRenderer() {

}

vkRenderer::~vkRenderer() {

}


// Initialization of the window.
int
vkRenderer::init(GLFWwindow *newWindow) {

    w = newWindow;

    try {
        createInstance();           // Create Instance
        setupDebugMessenger();      // Setup Validation Layer Messenger
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

    if(enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available.");
    }

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
            .pApplicationInfo       = &appInfo,
            .enabledLayerCount      = static_cast<uint32_t>(validationLayers.size()),
            .ppEnabledLayerNames    = validationLayers.data()
    };

    // Create list to hold instance extensions
    std::vector<const char *> vkExtensions = getRequiredExtensions();

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

// Check support for vulkan extensions.
bool
vkRenderer::checkInstanceExtensionSupport(std::vector<const char *> *checkExtensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // check if given extensions are in list of available extensions
    for (const auto &checkExtension: *checkExtensions) {
        if (!extensionInList(extensions, checkExtension)) return false;
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
    if(enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(w);
    glfwTerminate();

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
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilyList.data());

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
            .queueCreateInfoCount       = 1,                // Number of Queue Create Infos
            .pQueueCreateInfos          = &createInfo,        // List of queue create infos
            .enabledLayerCount          = 0,                  // Validation layers (deprecated in VK API 1.1+)
            .enabledExtensionCount      = 0,               // Logical Device extensions
            .ppEnabledExtensionNames    = nullptr,      // List of enabled logical device extensions
            .pEnabledFeatures           = &deviceFeatures     // Physical Device Features the Logical device will be using
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

bool
vkRenderer::checkValidationLayerSupport() {
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

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<const char*>
vkRenderer::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void vkRenderer::setupDebugMessenger() {
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr // Optional
    };

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult
vkRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vkRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                               const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
