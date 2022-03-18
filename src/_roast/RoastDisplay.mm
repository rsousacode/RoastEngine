#include "RoastDisplay.h"

#include "../mtl/MtlRender.h"
#include "../vk/VkRender.h"

void
RoastDisplay::initGlfw() {
    auto initResult = glfwInit();
    if (initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to setupCocoa");
    }
}
void RoastDisplay::handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void RoastDisplay::setupInput() {
    if (shouldHandleInput) {
        glfwSetKeyCallback(pGlfwWindow, handleInput);
    }
}


RDResult
RoastDisplay::createRenderer(const RDCreateInfo& info) {
    // TODO: do validation
    RType = info.displayEngine;
    windowWidth = info.windowWidth;
    windowHeight = info.windowHeight;
    start(info.windowTitle);
    return RD_SUCCESS;
}

int
RoastDisplay::start(const char *window) {
    VkRender vkRenderer;
    MtlRender mtlRenderer{};

    switch (RType) {
        case RE_OPENGL:
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            initGlfw();
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            pGlfwWindow = glfwCreateWindow(windowWidth, windowHeight, window, nullptr, nullptr);
            setupInput();

            while(!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
            }

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();

            return 0;

        case RE_VULKAN:
            vkRenderer.setupAdapter();
            vkRenderer.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = vkRenderer.GetGlfwWindow();
            setupInput();

            if (vkRenderer.init(pGlfwWindow) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
            }

            vkRenderer.cleanup();

            return 0;

        case RE_METAL:
            mtlRenderer.setupAdapter();
            mtlRenderer.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = mtlRenderer.GetGlfwWindow();
            setupInput();

            MTLClearColor color = MTLClearColorMake(0, 0, 0, 1);

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();

                @autoreleasepool {
                    color.red = (color.red > 1.0) ? 0 : color.red + 0.01;

                    id<CAMetalDrawable> surface = [mtlRenderer.swapchain nextDrawable];

                    MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
                    pass.colorAttachments[0].clearColor = color;
                    pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
                    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
                    pass.colorAttachments[0].texture = surface.texture;

                    id<MTLCommandBuffer> buffer = [mtlRenderer.queue commandBuffer];
                    id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:pass];
                    [encoder endEncoding];
                    [buffer presentDrawable:surface];
                    [buffer commit];
                }
            }

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();

            return 0;
    }

    return 0;
}
