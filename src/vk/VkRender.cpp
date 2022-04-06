#define GLFW_INCLUDE_VULKAN

#include "VkRender.h"

#import <stdexcept>
#import <iostream>
#import "vulkan/vulkan.h"
#include "../common/SGlfw.h"
#import <GLFW/glfw3.h>
#import <set>
#import <array>

VkRender::~VkRender() = default;


int
VkRender::init(GLFWwindow *newWindow) {
    glfwWindow = newWindow;
    try {
        createInstance();
        setupDebugMessenger();
        createSurface();
        createPhysicalDevices();
        createLogicalDevice();
        createMesh();
        createSwapchain();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        subscribeCommands();
        createSync();
    }
    catch (const std::runtime_error &e) {
        printf("ERROR %s\n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}


/*!
	Populates data about the DebugUtilsMessengerCreateInfo object necessary to set some flags
    used by the validation layers.
	@param createInfoExt  Reference relative of the VkDebugUtilsMessengerCreateInfoEXT we want to populate.
*/

void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfoExt) {
    createInfoExt = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback
    };
}

// Create Vulkan Instance
void
VkRender::createInstance() {
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
            .ppEnabledLayerNames    = validationLayers.data(),
    };

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

    if (enableValidationLayers && !hasValidationLayersSupport()) {

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    // Create list to hold instance extensions
    std::vector<const char *> vkExtensions = getRequiredExtensions();

    // Check instance extensions support
    if (!supportsInstanceExtensions(&vkExtensions)) {
        throw std::runtime_error("There's no extension supported from this platform.");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(vkExtensions.size());
    createInfo.ppEnabledExtensionNames = vkExtensions.data();



    // Create instance
    VkResult r = vkCreateInstance(&createInfo, nullptr, &instance);

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
    vkDeviceWaitIdle(mainDevice.logicalDevice);
    firstMesh.destroyVertexBuffer();
    destroySemaphores();
    vkDestroyCommandPool(mainDevice.logicalDevice, graphicsCommandPool, nullptr);
    cleanFramebuffers();
    vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);
    cleanupImages();
    vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchainKhr, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

}

void
VkRender::cleanupImages() {
    for (auto image: swapChainImages) {
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
    for (const auto &device: deviceList) {
        if (deviceIsSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

uint32_t
VkRender::getDeviceCount() const {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    return deviceCount;
}

std::vector<VkPhysicalDevice>
VkRender::getdeviceList(uint32_t &deviceCount) const {
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

    if (extensionsSupported) {
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
    for (const auto &queueFamily: queueFamilyList) {

        // Check if queue is graphics
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamilyIndex = i;
        }

        // check if queue family supports presentation (can be both)
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);

        if (queueFamily.queueCount > 0 && presentationSupport) {
            indices.presentationFamilyIndex = i;
        }

        if (indices.isValid()) {
            break;
        }

        i++;

    }
    return indices;
}


std::vector<VkDeviceQueueCreateInfo>
VkRender::generateQueueCreateInfos() {

    std::set<uint32_t> queueFamilyIndexes = {indices.presentationFamilyIndex, indices.graphicsFamilyIndex};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.;

    for (uint32_t queueFamilyIndex: queueFamilyIndexes) {
        // Queue the logical device needs to create and info to do so
        VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueFamilyIndex,
                .queueCount = 1,                                       // Number of queues to create
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
    VkPhysicalDeviceFeatures deviceFeatures{};

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

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Logical Device!");
    }

    // from given logical device of given queue family of given queue index (0 since only one queue)
    // assign Queue reference in given graphicsQueue
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamilyIndex, 0, &presentationQueue);
}

bool
VkRender::hasValidationLayersSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check if all of the layers in validationLayers exist in available layers

    for (const char *layerName: validationLayers) {
        bool layerFound = false;
        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) return false;
    }
    return true;
}


std::vector<const char *>
VkRender::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void
VkRender::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

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
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void
VkRender::createSurface() {

    auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a surface");
    }
}

std::vector<VkExtensionProperties>
VkRender::generateExtensionProperties(VkPhysicalDevice device) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) {
        return std::vector<VkExtensionProperties>(0);
    }

    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,
                                         &extensionCount, extensionProperties.data());

    return extensionProperties;

}

bool
VkRender::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) {
        return false;
    }

    std::vector<VkExtensionProperties> extensionProperties = generateExtensionProperties(device);

    bool hasExtension = false;

    for (const auto &deviceExtension: deviceExtensions) {

        for (const auto &extension: extensionProperties) {
            if (strcmp(deviceExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) {
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
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);


    if (formatCount != 0) {
        details.surfaceFormatKhr.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormatKhr.data());
    }
    // Get surface present mode

    uint32_t presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

    if (presentationCount != 0) {
        details.presentModeKhr.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, details.presentModeKhr.data());
    }

    return details;
}


// Creates a Swap Chain
void
VkRender::createSwapchain() {
    // Get swap chain details so we can pick best settings
    SwapChainDetails swapchainDetails = getSwapChainDetails(mainDevice.physicalDevice);
    VkSurfaceFormatKHR surfaceFormat = getSurfaceFormat(swapchainDetails.surfaceFormatKhr);
    VkPresentModeKHR presentMode = getPresentationMode(swapchainDetails.presentModeKhr);
    VkExtent2D extent = getSwapExtent(swapchainDetails.surfaceCapabilities);

    // Check many images in the swap chain. 1 More to allow triple buffering

    uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;

    // If imageCount is higher than max, clamp to max
    // If 0, limitless
    if (swapchainDetails.surfaceCapabilities.maxImageCount > 0 &&
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
    if (indices.graphicsFamilyIndex != indices.presentationFamilyIndex) {
        uint32_t queueFamilyIndexes[] = {
                (uint32_t) indices.graphicsFamilyIndex,
                (uint32_t) indices.presentationFamilyIndex
        };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;   // Share handling
        swapchainCreateInfo.queueFamilyIndexCount = 2;                      // Number of queues to share images between
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndexes;       // Array of queues to share between
    }

    auto result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfo, nullptr, &swapchainKhr);

    if (result != VK_SUCCESS) {
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
    for (VkImage image: images) {

        SwapchainImage swapchainImage = {
                .image = image,
                .imageView = getImageView(image, swapChainImageFormat,
                                          VK_IMAGE_ASPECT_COLOR_BIT)    // Aspect of the image we want to view
        };

        swapChainImages.push_back(swapchainImage);
    }
}

void VkRender::createRenderPass() {
    // Colour attachment of render pass
    VkAttachmentDescription colourAttachment = {};
    colourAttachment.format = swapChainImageFormat;                        // Format to use for attachment
    colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                    // Number of samples to write for multisampling
    colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                // Describes what to do with attachment before rendering
    colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;            // Describes what to do with attachment after rendering
    colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // Describes what to do with stencil before rendering
    colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // Describes what to do with stencil after rendering

    // Framebuffer data will be stored as an image, but images can be given different data layouts
    // to give optimal use for certain operations
    colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;            // Image data layout before render pass starts
    colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;        // Image data layout after render pass (to change to)

    // Attachment reference uses an attachment index that refers to index in the attachment list passed to renderPassCreateInfo
    VkAttachmentReference colourAttachmentReference = {};
    colourAttachmentReference.attachment = 0;
    colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Information about a particular subpass the Render Pass is using
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;        // Pipeline type subpass is to be bound to
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colourAttachmentReference;

    // Need to determine when layout transitions occur using subpass dependencies
    std::array<VkSubpassDependency, 2> subpassDependencies{};

    // Conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    // Transition must happen after...
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;                        // Subpass index (VK_SUBPASS_EXTERNAL = Special value meaning outside of renderpass)
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;        // Pipeline stage
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;                // Stage access mask (memory access)
    // But must happen before...
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0;


    // Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    // Transition must happen after...
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;;
    // But must happen before...
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = 0;

    // Create info for Render Pass
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colourAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Render Pass!");
    }
}

#include <fstream>

std::vector<char> VkRender::readShaderFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to read shader compiled file.");
    }

    long fileSize = file.tellg();

    std::vector<char> fileBuffer(fileSize);

    file.seekg(0);

    file.read(fileBuffer.data(), fileSize);

    file.close();

    return fileBuffer;
}

VkShaderModule VkRender::createShaderModule(const std::vector<char> &code) {
    // Shader Module creation information
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();                                        // Size of code
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());        // Pointer to code (of uint32_t pointer type)

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(mainDevice.logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a shader module!");
    }

    return shaderModule;
}

void VkRender::createGraphicsPipeline() {
    // Read in SPIR-V code of shaders
    auto vertexShaderCode = readShaderFile("../src/vk/shaders/bin/vert.spv");
    auto fragmentShaderCode = readShaderFile("../src/vk/shaders/bin/frag.spv");

    // Create Shader Modules
    VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

    // -- SHADER STAGE CREATION INFORMATION --
    // Vertex Stage creation information
    VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,                // Shader Stage name
            .module = vertexShaderModule,                        // Shader module to be used by stage
            .pName = "main"                                    // Entry point in to shader
    };


    // Fragment Stage creation information
    VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,                // Shader Stage name
            .module = fragmentShaderModule,                        // Shader module to be used by stage
            .pName = "main"                                    // Entry point in to shader
    };

    // Put shader stage creation info in to array
    // Graphics Pipeline creation info requires array of shader stage creates
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderCreateInfo, fragmentShaderCreateInfo};

    // How the data for a single vertex (including info such as position, color, texture coords, normals etc) as a whole
    // Each vertex could have multiple attributes
    VkVertexInputBindingDescription bindingDescription = {
            .binding = 0,                                               // Binding multiple streams (defines index)
            .stride = sizeof(Vertex),                                   // Size of the object
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,               // How to move data after each vertex
                                                                            // VK_VERTEX_INpUT-RATE_INDEX : Move to the next vertex
                                                                            // VK_VERTEX_INPUT_RATE_INSTANCE : Move to a vertex for the next instance
    };

    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

    // Position attribute
    attributeDescriptions[0] = {
            .location = 0,                          // Samer as color attachment in location in shader
            .binding = 0,                           // Same value as the shader binding
            .format = VK_FORMAT_R32G32B32_SFLOAT,   // Format of data (and helps in defining size of the data)
            .offset = offsetof(Vertex, pos)                 // Offset till reaching pos in memory
    };



    // -- VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,            // List of Vertex Binding Descriptions (data spacing/stride information)
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data(),        // List of Vertex Attribute Descriptions (data format and where to bind to/from)
    };

    // -- INPUT ASSEMBLY --
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,        // Primitive type to assemble vertices as
            .primitiveRestartEnable = VK_FALSE,                    // Allow overriding of "strip" topology to start new primitives
    };

    // -- VIEWPORT & SCISSOR --
    // Create a viewport info struct
    VkViewport viewport = {
            .x = 0.0f,                                    // x start coordinate
            .y = 0.0f,                                    // y start coordinate
            .width = (float) swapChainExtent.width,        // width of viewport
            .height = (float) swapChainExtent.height,    // height of viewport
            .minDepth = 0.0f,                            // min framebuffer depth
            .maxDepth = 1.0f,                            // max framebuffer depth
    };


    // Create a scissor info struct
    VkRect2D scissor = {
            .offset = {0, 0},                            // Offset to use region from
            .extent = swapChainExtent,                    // Extent to describe region to use, starting at offset
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
    };

    // -- DYNAMIC STATES --
    // Dynamic states to enable
    //std::vector<VkDynamicState> dynamicStateEnables;
    //dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);	// Dynamic Viewport : Can resize in command buffer with vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
    //dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);	// Dynamic Scissor	: Can resize in command buffer with vkCmdSetScissor(commandbuffer, 0, 1, &scissor);

    //// Dynamic State creation info
    //VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    //dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    //dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    //dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();


    // -- RASTERIZER --
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,            // Change if fragments beyond near/far planes are clipped (default) or clamped to plane
            .rasterizerDiscardEnable = VK_FALSE,    // Whether to discard data and skip rasterizer. Never creates fragments, only suitable for pipeline without framebuffer output
            .polygonMode = VK_POLYGON_MODE_FILL,    // How to handle filling points between vertices
            .lineWidth = 1.0f,                        // How thick lines should be when drawn
            .cullMode = VK_CULL_MODE_BACK_BIT,        // Which face of a tri to cull
            .frontFace = VK_FRONT_FACE_CLOCKWISE,    // Winding to determine which side is front
            .depthBiasEnable = VK_FALSE            // Whether to add depth bias to fragments (good for stopping "shadow acne" in shadow mapping)
    };


    // -- MULTISAMPLING --
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .sampleShadingEnable = VK_FALSE,                    // Enable multisample shading or not
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT    // Number of samples to use per fragment
    };

    // -- BLENDING --
    // Blending decides how to blend a new colour being written to a fragment, with the old value

    // Blend Attachment State (how blending is handled)
    VkPipelineColorBlendAttachmentState colourState = {
            .blendEnable = VK_TRUE,                                                    // Enable blending
            //uses equation: (srcColorBlendFactor * new colour) colorBlendOp (dstColorBlendFactor * old colour)
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT    // Colours to apply blending to
                              | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };


    // Summarised: (1 * new alpha) + (0 * old alpha) = new alpha
    VkPipelineColorBlendStateCreateInfo colourBlendingCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,                // Alternative to calculations is to use logical operations
            .attachmentCount = 1,
            .pAttachments = &colourState
    };

    // -- PIPELINE LAYOUT (TODO: Apply Future Descriptor Set Layouts) --
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
    };

    // Create Pipeline Layout
    VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &pipelineLayoutCreateInfo, nullptr,
                                             &pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Pipeline Layout!");
    }


    // -- DEPTH STENCIL TESTING --
    // TODO: Set up depth stencil testing


    // -- GRAPHICS PIPELINE CREATION --
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,                                    // Number of shader stages
            .pStages = shaderStages,                            // List of shader stages
            .pVertexInputState = &vertexInputCreateInfo,        // All the fixed function pipeline states
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportStateCreateInfo,
            .pRasterizationState = &rasterizerCreateInfo,
            .pMultisampleState = &multisamplingCreateInfo,
            .pDepthStencilState = VK_NULL_HANDLE,
            .pColorBlendState = &colourBlendingCreateInfo,
            .pDynamicState = VK_NULL_HANDLE,
            .layout = pipelineLayout,                            // Pipeline Layout pipeline should use
            .renderPass = renderPass,                            // Render pass description the pipeline is compatible with
            .subpass = 0,                                        // Subpass of render pass to use with pipeline
            .basePipelineHandle = VK_NULL_HANDLE,    // Existing pipeline to derive from...
            .basePipelineIndex = -1                // or index of pipeline being created to derive from (in case creating multiple at once)
    };

    // Create Graphics Pipeline
    result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                       &graphicsPipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Graphics Pipeline!");
    }

    // Destroy Shader Modules, no longer needed after Pipeline created
    vkDestroyShaderModule(mainDevice.logicalDevice, fragmentShaderModule, nullptr);
    vkDestroyShaderModule(mainDevice.logicalDevice, vertexShaderModule, nullptr);
}

// Chooses surface Format. Defaults:
// Format       :  VK_FORMAT_R8G8B8A8_UNORM
// Alternative  :  VK_FORMAT_B8G8R8A8_UNORM
// ColorSpace   :  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR
VkRender::getSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &format: formats) {
        if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
            && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats[0];
}

VkPresentModeKHR
VkRender::getPresentationMode(std::vector<VkPresentModeKHR> presentationModes) {
    for (const auto &presentationMode: presentationModes) {
        if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentationMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VkRender::getSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities) const {
    // If current extent is at numeric limits, then extent can vary. Otherwise is the size of the window
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
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
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create an Image view!");
    }
    return imageView;
}

void
VkRender::initWindow(const char *wName, int width, int height, Vector4 &color) {
    clearColor = color;
    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Unable to start GLFW in Vulkan context");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    SGlfw::UseEngineGlfwWindowHints();
    glfwWindow = glfwCreateWindow(width, height, wName, nullptr, nullptr);
}

GLFWwindow *
VkRender::GetGlfwWindow() const {
    return glfwWindow;
}

void
VkRender::createFramebuffers() {
    framebuffers.resize(swapChainImages.size());

    for (size_t i = 0; i < framebuffers.size(); i++) {

        std::array<VkImageView, 1> attachments = {
                swapChainImages[i].imageView
        };

        VkFramebufferCreateInfo framebufferCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = renderPass,
                .attachmentCount = static_cast<uint32_t >(attachments.size()),
                .pAttachments = attachments.data(),
                .width = swapChainExtent.width,
                .height = swapChainExtent.height,
                .layers = 1
        };

        VkResult result = vkCreateFramebuffer(mainDevice.logicalDevice, &framebufferCreateInfo, nullptr,
                                              &framebuffers[i]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error creating frame buffer");
        }

    }
}

void
VkRender::cleanFramebuffers() {
    for (auto framebuffer: framebuffers) {
        vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
    }
}

void
VkRender::createCommandPool() {
    VkCommandPoolCreateInfo poolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = indices.graphicsFamilyIndex,
    };

    VkResult result = vkCreateCommandPool(mainDevice.logicalDevice, &poolCreateInfo, nullptr, &graphicsCommandPool);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }


}

void
VkRender::createCommandBuffers() {
    commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo cbAllocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = graphicsCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = static_cast<uint32_t > (commandBuffers.size())
    };

    VkResult result = vkAllocateCommandBuffers(mainDevice.logicalDevice, &cbAllocInfo, commandBuffers.data());

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void
VkRender::subscribeCommands() {
    VkCommandBufferBeginInfo bufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = VK_NULL_HANDLE,
    };

    VkRect2D renderArea = {
            .offset = {0, 0},
            .extent = swapChainExtent
    };

    // rgba
    VkClearValue clearValues[] = {
            {clearColor.x, clearColor.y, clearColor.z, clearColor.w}
    };

    // Begin render pass
    VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,   // Size of region to run render pass
            .renderArea = renderArea,   // Starting point of render pass in px
            .clearValueCount = 1,
            .pClearValues = clearValues, // TODO Depth attachment clear value
    };

    for (size_t i = 0; i < commandBuffers.size(); i++) {

        // Use render pass with slightly changed frame buffer
        renderPassBeginInfo.framebuffer = framebuffers[i];

        auto result = vkBeginCommandBuffer(commandBuffers[i], &bufferBeginInfo);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error: begin command buffer");
        }

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            // bin pipeline
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            VkBuffer vertexBuffer[] = { firstMesh.vertexBuffer }; // Buffers to bind
            VkDeviceSize offsets[] = { 0 };                           // Offsets into buffers being bound
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffer, offsets);

            // execute pipeline
            vkCmdDraw(commandBuffers[i], firstMesh.vertexCount, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        result = vkEndCommandBuffer(commandBuffers[i]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error: end command buffer");
        }
    }
}

void
VkRender::Draw() {
    // Get next available image to display to and set something to signal when we're finished with the image
    // Submit command buffer to queue for execution, making sure it waits for the image to be signalled as available before drawing
    // and signals when it has finished rendering
    // Present to screen when it has signalled finished rendering

    // Wait for given fence signal from last display before continuing
    vkWaitForFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    vkResetFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(mainDevice.logicalDevice, swapchainKhr, std::numeric_limits<uint64_t>::max(),
                          imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

    VkPipelineStageFlags stageFlags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailable[currentFrame],
            .pWaitDstStageMask = stageFlags,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffers[imageIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinish[currentFrame]
    };

    VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFences[currentFrame]);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer to queue!");
    }

    VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinish[currentFrame],
            .swapchainCount = 1,
            .pSwapchains = &swapchainKhr,
            .pImageIndices = &imageIndex
    };

    result = vkQueuePresentKHR(presentationQueue, &presentInfo);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vk presentation queue");
    }

    // Get Next frame (use % MAX_FRAME_DRAWS to keep value below MAX_FRAME_DRAWS)
    currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void
VkRender::createSync() {
    imageAvailable.resize(MAX_FRAME_DRAWS);
    renderFinish.resize(MAX_FRAME_DRAWS);
    drawFences.resize(MAX_FRAME_DRAWS);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < MAX_FRAME_DRAWS; i++) {
        if (vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailable[i]) != VK_SUCCESS
            ||
            vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinish[i]) != VK_SUCCESS
            || vkCreateFence(mainDevice.logicalDevice, &fenceCreateInfo, nullptr, &drawFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore and/or fence");
        }
    }

}

void
VkRender::destroySemaphores() {
    for (size_t i = 0; i < MAX_FRAME_DRAWS; i++) {
        vkDestroySemaphore(mainDevice.logicalDevice, renderFinish[i], nullptr);
        vkDestroySemaphore(mainDevice.logicalDevice, imageAvailable[i], nullptr);
        vkDestroyFence(mainDevice.logicalDevice, drawFences[i], nullptr);
    }
}

void
VkRender::createMesh() {
    std::vector<Vertex> meshVertices = {
            {{0.0, -0.5, 0.0}},
             {{0.5, 0.5, 0.0}},
             {{-0.5, 0.5, 0.5}}
            };

    firstMesh = VkMesh(mainDevice.physicalDevice, mainDevice.logicalDevice, &meshVertices);

}

VkRender::VkRender() {

}

