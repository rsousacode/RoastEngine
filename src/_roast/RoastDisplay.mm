#include "RoastDisplay.h"
#include "../mtl/mtlRenderer.h"

RoastDisplay::RoastDisplay(const RE_Type rType) {
    RType = rType;
}

void
RoastDisplay::initGlfw() {
    auto initResult = glfwInit();
    if(initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to setupCocoa");
    }
}


int
RoastDisplay::start(const char *windowTitle) {
    vkRenderer vkRenderer;
    mtlRenderer metalRenderer;

    switch (RType) {
        case RE_OPENGL:
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            initGlfw();
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            pGlfwWindow = glfwCreateWindow(WIDTH, HEIGHT, windowTitle, nullptr, nullptr);
            while(!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();

            }

            return 0;

        case RE_VULKAN:
            #define GLFW_INCLUDE_VULKAN
            initGlfw();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            pGlfwWindow = glfwCreateWindow(WIDTH, HEIGHT, windowTitle, nullptr, nullptr);
            if (vkRenderer.init(pGlfwWindow) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
            }

            vkRenderer.cleanup();

            return 0;

        case RE_METAL:
            const id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
            const id<MTLCommandQueue> queue = [gpu newCommandQueue];
            CAMetalLayer *swapchain = [CAMetalLayer layer];
            swapchain.device = gpu;
            swapchain.opaque = YES;

            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, windowTitle, NULL, NULL);
            NSWindow *nswindow = glfwGetCocoaWindow(window);
            nswindow.contentView.layer = swapchain;
            nswindow.contentView.wantsLayer = YES;

            //glfwSetKeyCallback(window, quit);
            MTLClearColor color = MTLClearColorMake(0, 0, 0, 1);

            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();

                @autoreleasepool {
                    color.red = (color.red > 1.0) ? 0 : color.red + 0.01;

                    id<CAMetalDrawable> surface = [swapchain nextDrawable];

                    MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
                    pass.colorAttachments[0].clearColor = color;
                    pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
                    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
                    pass.colorAttachments[0].texture = surface.texture;

                    id<MTLCommandBuffer> buffer = [queue commandBuffer];
                    id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:pass];
                    [encoder endEncoding];
                    [buffer presentDrawable:surface];
                    [buffer commit];
                }
            }

            glfwDestroyWindow(window);
            glfwTerminate();

            return 0;
    }

    return 0;
}
