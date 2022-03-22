#ifndef ROASTENGINE_VKRENDER_H
#define ROASTENGINE_VKRENDER_H
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include "utils.h"

#define GLFW_INCLUDE_VULKAN

class VkRender {

public:
    VkRender();
    int     init(GLFWwindow *newWindow);
    void    cleanup();
    ~VkRender();

    float clearColor[4];

    bool setupAdapter();

    void initWindow(const char *wName, int width, int height);

    GLFWwindow                  *glfwWindow{};

    GLFWwindow *GetGlfwWindow() const;

    void Draw();

private:

    void cleanupImages();


    // Vulkan Instance
    VkInstance                  instance{};

    struct {
        VkPhysicalDevice        physicalDevice;
        VkDevice                logicalDevice;
    } mainDevice{};

    VkQueue                     graphicsQueue{};
    VkQueue                     presentationQueue{};
    VkSurfaceKHR                surface{};
    VkSwapchainKHR              swapchainKhr{};
    std::vector<SwapchainImage> swapChainImages;

    VkFormat                    swapChainImageFormat{};
    VkExtent2D                  swapChainExtent{};
    VkSemaphore                 imageAvailable;
    VkSemaphore                 renderFinish;


    QueueFamilyIndexes indices;

    // Instance Support
    void createInstance();

    // Devices Support
    void                            createPhysicalDevices();
    void                            createLogicalDevice();
    void                            createSurface();
    void                            createRenderPass();
    void                            createGraphicsPipeline();
    void                            createFramebuffers();
    void                            createCommandPool();
    void                            createCommandBuffers();

    void                            subscribeCommands();

    void                            createSync();

    VkShaderModule                  createShaderModule(const std::vector<char> &code);
    VkPipeline                      graphicsPipeline;
    VkPipelineLayout                pipelineLayout;
    VkRenderPass                    renderPass;
    std::vector<VkFramebuffer>      framebuffers;
    std::vector<VkCommandBuffer>    commandBuffers;
    VkCommandPool                   graphicsCommandPool;

    // Swapchain
    void                createSwapchain();
    VkSurfaceFormatKHR  getSurfaceFormat        (const std::vector<VkSurfaceFormatKHR> &formats);
    VkExtent2D          getSwapExtent           (const VkSurfaceCapabilitiesKHR &surfaceCapabilities) const;
    VkPresentModeKHR    getPresentationMode     (std::vector<VkPresentModeKHR> presentationModes);

    VkImageView         getImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

    // Extension Support
    bool                                    supportsInstanceExtensions  (std::vector<const char*> *checkExtensions);
    bool                                    listHasExtension                (std::vector<VkExtensionProperties> &extensions,
                                                                             const char *const &checkExtension) const;
    std::vector<VkDeviceQueueCreateInfo>    generateQueueCreateInfos();
    std::vector<VkExtensionProperties>      generateExtensionProperties(VkPhysicalDevice device);
    std::vector<VkExtensionProperties>      getExtensionProperties(uint32_t &extensionCount) const;
    std::vector<VkPhysicalDevice>           getdeviceList(uint32_t &deviceCount) const;

    [[nodiscard]] uint32_t getDeviceCount() const;

    // Device Support
    bool                        deviceIsSuitable(VkPhysicalDevice device);
    bool                        checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndexes          getQueueFamilies(VkPhysicalDevice device);
    SwapChainDetails            getSwapChainDetails(VkPhysicalDevice device);
    static std::vector<char>     readShaderFile(const std::string &filename);

    // Validation layers
    // TODO: creation and clean handling

    // Debug Implementation (Validation Layers)
    VkDebugUtilsMessengerEXT debugMessenger{};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };


    bool hasValidationLayersSupport();

    std::vector<const char *> getRequiredExtensions() const;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void setupDebugMessenger();

    void cleanFramebuffers();
};

#endif //ROASTENGINE_VKRENDER_H
