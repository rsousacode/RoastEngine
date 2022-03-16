#define GLFW_INCLUDE_VULKAN

#include <stdexcept>
#include <iostream>
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include <set>
#include "VkRender.h"

VkRender::VkRender() = default;
VkRender::~VkRender() = default;

// Initialization of the window.
int
VkRender::init(GLFWwindow *newWindow) {
    glfwWindow = newWindow;
    try {
        createInstance();
        setupDebugMessenger();
        createSurface();
        createPhysicalDevices();
        createLogicalDevice();
        createSwapchain();
    }
    catch (const std::runtime_error &e) {
        printf("ERROR %s\n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}

// Create Vulkan Instance
void
VkRender::createInstance() {
    if(enableValidationLayers && !hasValidationLayersSupport()) {
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
    if (!supportsInstanceExtensions(&vkExtensions)) {
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
VkRender::supportsInstanceExtensions(std::vector<const char *> *checkExtensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions = getExtensionProperties(extensionCount);

    // check if given extensions are in list of available extensions
    for (const auto &checkExtension: *checkExtensions) {
        if (!listHasExtension(extensions, checkExtension)) return false;
    }

    return true;
}

std::vector<VkExtensionProperties> VkRender::getExtensionProperties(uint32_t &extensionCount) const {
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

// Check if c_string (extension) is in list
bool
VkRender::listHasExtension(std::vector<VkExtensionProperties> &extensions, const char *const &checkExtension) const {
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
VkRender::cleanup() {
    cleanupImages();
    vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchainKhr, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    if(enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

}

void
VkRender::cleanupImages() {
    for (auto image : swapChainImages) {
        vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
    }
}

// Enumerate Physical Devices
void
VkRender::createPhysicalDevices() {
    uint32_t deviceCount = getDeviceCount();
    // If no devices available, none support Vulkan
    if (deviceCount == 0) {
        throw std::runtime_error("Can't find GPUs that support Vulkan instance.");
    }

    std::vector<VkPhysicalDevice> deviceList = getdeviceList(deviceCount);

    // Get actual physical device
    for(const auto &device : deviceList) {
        if(deviceIsSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

uint32_t VkRender::getDeviceCount() const {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    return deviceCount;
}

std::vector<VkPhysicalDevice> VkRender::getdeviceList(uint32_t &deviceCount) const {
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
    return deviceList;
}

// Check if device is suitable to run Vulkan
bool
VkRender::deviceIsSuitable(VkPhysicalDevice device) {

    /*
    // Info about the device (ID, name, type, vendor, etc)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // Information about what the device can do (geo shader, tess shader, wide lines, etc)

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
     */

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainValid = false;

    if(extensionsSupported) {
        SwapChainDetails swapChainDetails = getSwapChainDetails(device);
        swapChainValid = !swapChainDetails.presentModeKhr.empty() && !swapChainDetails.surfaceFormatKhr.empty();
    }

    return indices.isValid() && extensionsSupported && swapChainValid;
}

// Get all Queue Family Property info for the given device
QueueFamilyIndexes
VkRender::getQueueFamilies(VkPhysicalDevice device) {
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

        // Check if queue is graphics
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamilyIndex = i;
        }

        // check if queue family supports presentation (can be both)
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);

        if(queueFamily.queueCount > 0 && presentationSupport) {
            indices.presentationFamilyIndex = i;
        }

        if (indices.isValid()) {
            break;
        }
        
        i++;

    }
    return  indices;
}


std::vector<VkDeviceQueueCreateInfo>
VkRender::generateQueueCreateInfos() {

    std::set<uint32_t> queueFamilyIndexes = {indices.presentationFamilyIndex, indices.graphicsFamilyIndex};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.;

    for(uint32_t queueFamilyIndex : queueFamilyIndexes) {
        // Queue the logical device needs to create and info to do so
        VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueFamilyIndex,
                .queueCount = 1 ,                                       // Number of queues to create
                .pQueuePriorities = &queuePriority                   // QueueFamily priority (1 = highest priority)

        };

        queueCreateInfos.push_back(queueCreateInfo);

    }

    return queueCreateInfos;
}

void
VkRender::createLogicalDevice() {

    indices = getQueueFamilies(mainDevice.physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = generateQueueCreateInfos();

    // Physical Device Features the Logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures {};

    // Information to create logical device
    VkDeviceCreateInfo deviceCreateInfo = {
            .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount       = static_cast<uint32_t>(queueCreateInfos.size()), // Number of Queue Create Infos
            .pQueueCreateInfos          = queueCreateInfos.data(),                          // List of queue create infos
            .enabledLayerCount          = 0,
            .enabledExtensionCount      = static_cast<uint32_t>(deviceExtensions.size()),// Logical Device extensions
            .ppEnabledExtensionNames    = deviceExtensions.data(),                    // Enabled logical device extensions
            .pEnabledFeatures           = &deviceFeatures                                    // Physical Device Features the logical
                                                                                                             //  device will be using
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
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamilyIndex,0, &presentationQueue);
}

bool
VkRender::hasValidationLayersSupport() {
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

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<const char*>
VkRender::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void
VkRender::setupDebugMessenger() {
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
VkRender::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                       const VkAllocationCallbacks *pAllocator,
                                       VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void
VkRender::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                        const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void
VkRender::createSurface() {

    auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a surface");
    }
}

std::vector<VkExtensionProperties>
VkRender::generateExtensionProperties(VkPhysicalDevice device ) {
    uint32_t  extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0) {
        return std::vector<VkExtensionProperties> (0);
    }

    std::vector<VkExtensionProperties> extensionProperties (extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,
                                         &extensionCount, extensionProperties.data());

    return extensionProperties;

}

bool
VkRender::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t  extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0) {
        return false;
    }

    std::vector<VkExtensionProperties> extensionProperties  = generateExtensionProperties(device);

    bool hasExtension = false;

    for(const auto &deviceExtension : deviceExtensions) {

        for(const auto &extension : extensionProperties) {
            if (strcmp(deviceExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }

        if(!hasExtension) {
            return false;
        }
    }

    return hasExtension;

}

SwapChainDetails
VkRender::getSwapChainDetails(VkPhysicalDevice device) {
    SwapChainDetails details;

    // Get surface capabilities for the given surface on the physical device
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surfaceCapabilities);
    uint32_t  formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);


    if(formatCount != 0) {
        details.surfaceFormatKhr.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormatKhr.data());
    }
    // Get surface present mode

    uint32_t  presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

    if(presentationCount != 0) {
        details.presentModeKhr.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, details.presentModeKhr.data());
    }

    return details;
}


// Creates a Swap Chain
void
VkRender::createSwapchain() {
    // Get swap chain details so we can pick best settings
    SwapChainDetails swapchainDetails   = getSwapChainDetails(mainDevice.physicalDevice);
    VkSurfaceFormatKHR surfaceFormat    = getSurfaceFormat(swapchainDetails.surfaceFormatKhr);
    VkPresentModeKHR presentMode        = getPresentationMode(swapchainDetails.presentModeKhr);
    VkExtent2D extent                   = getSwapExtent(swapchainDetails.surfaceCapabilities);

    // Check many images in the swap chain. 1 More to allow triple buffering

    uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;

    // If imageCount is higher than max, clamp to max
    // If 0, limitless
    if(swapchainDetails.surfaceCapabilities.maxImageCount > 0 &&
       swapchainDetails.surfaceCapabilities.maxImageCount < imageCount) {
        imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface,
            .minImageCount = imageCount,                         // Minimum images in swap chain
            .imageFormat = surfaceFormat.format,                   // Swapchain format
            .imageColorSpace = surfaceFormat.colorSpace,        // Color space
            .imageExtent = extent,                                  // Image extents
            .imageArrayLayers = 1,                              // Number of layers for each image in chain
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,      // Attachment images usage
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = swapchainDetails                       // Transform to perform on swap chain
                                            .surfaceCapabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // Handling blending images with extern graphics (other windows)
            .presentMode = presentMode,                                         // Presentation mode
            .clipped = VK_TRUE,                                                 // Whether to clip parts of image not in view (win offscreen, etc.)
            .oldSwapchain = VK_NULL_HANDLE                                      // Link old one to quicly hand over responsabilities
    };

    // If graphics and presentation families are different, then swapchainKhr must let images be shared between families
    if(indices.graphicsFamilyIndex != indices.presentationFamilyIndex) {
        uint32_t queueFamilyIndexes[] = {
                (uint32_t) indices.graphicsFamilyIndex,
                (uint32_t) indices.presentationFamilyIndex
        };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;   // Share handling
        swapchainCreateInfo.queueFamilyIndexCount = 2;                      // Number of queues to share images between
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndexes;       // Array of queues to share between
    }

    auto result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfo, nullptr, &swapchainKhr);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Swapchain!");
    }

    // Store for later reference
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    // GetSwapChain Images
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchainKhr, &swapChainImageCount, nullptr);
    std::vector<VkImage> images(swapChainImageCount);
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchainKhr, &swapChainImageCount, images.data());

    // Store image handling
    for (VkImage image : images) {

        SwapchainImage swapchainImage = {
                .image = image,
                .imageView = getImageView(image, swapChainImageFormat,
                                          VK_IMAGE_ASPECT_COLOR_BIT)    // Aspect of the image we want to view
        };

        swapChainImages.push_back(swapchainImage);
    }
}


// Chooses surface Format. Defaults:
// Format       :  VK_FORMAT_R8G8B8A8_UNORM
// Alternative  :  VK_FORMAT_B8G8R8A8_UNORM
// ColorSpace   :  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR
VkRender::getSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
    if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_R8G8B8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    for(const auto &format : formats) {
        if((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
        && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats[0];
}

VkPresentModeKHR
VkRender::getPresentationMode(std::vector<VkPresentModeKHR> presentationModes) {
    for (const auto &presentationMode : presentationModes) {
        if( presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentationMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VkRender::getSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities) const {
    // If current extent is at numeric limits, then extent can vary. Otherwise is the size of the window
    if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return surfaceCapabilities.currentExtent;
    } else {
        // GetWindowSize
        int width, height;

        uint32_t newWidth, newHeight;

        glfwGetFramebufferSize(glfwWindow, &width, &height);

        newWidth = std::max(surfaceCapabilities.minImageExtent.width,
                            std::min(surfaceCapabilities.maxImageExtent.width, static_cast<uint32_t>(width)));

        newHeight = std::max(surfaceCapabilities.minImageExtent.height,
                            std::min(surfaceCapabilities.maxImageExtent.height, static_cast<uint32_t>(height)));

        // Create new extent using window size
        VkExtent2D newExtent = {
                .width = newWidth,
                .height = newHeight
        };

        // Make sure boundaries by clamping value
        return newExtent;

    }
}

VkImageView
VkRender::getImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const {
    VkImageViewCreateInfo viewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,                                                   // Image to create view for
            .viewType = VK_IMAGE_VIEW_TYPE_2D,                      // Type of the image
            .format = format,                                                // Format of image data
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,      // Allows remapping rgba components
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = aspectFlags, // Which aspect of image to view
                                                                                 // eg. COLOR_BIT for view color
            .subresourceRange.baseMipLevel = 0,       // Start mipmap level to view from
            .subresourceRange.levelCount = 1,           // Number of mipmap levels to view
            .subresourceRange.baseArrayLayer = 0,   // Start array level to view from
            .subresourceRange.layerCount = 1           // Number of array levels to view
    };

    VkImageView imageView;
    VkResult result = vkCreateImageView(mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create an Image view!");
    }
    return imageView;
}

bool VkRender::setupAdapter() {
    if(glfwInit() == GLFW_FALSE) {
        return false;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    return true;
}

void VkRender::initWindow(const char *wName, int width, int height) {
    glfwWindow = glfwCreateWindow(width, height, wName, nullptr, nullptr);
}

GLFWwindow *VkRender::GetGlfwWindow() const {
    return glfwWindow;
}
