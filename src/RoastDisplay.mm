#include "RoastDisplay.h"

#import "mtl/MtlRender.h"
#import "vk/VkRender.h"
#import "ogl/OglRender.h"
#import "RMath.h"


void
RoastDisplay::handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void
RoastDisplay::setupInput() {
    if (shouldHandleInput) {
        glfwSetKeyCallback(pGlfwWindow, handleInput);
    }
}

void
RoastDisplay::Init(const RDCreateInfo& info, WindowCreateInfo &wCreateInfo) {
    RType = info.typeCompatibility.Type;
    windowWidth = info.windowWidth;
    windowHeight = info.windowHeight;
    start(info.windowTitle, wCreateInfo);
}

void
RoastDisplay::start(const char *window , WindowCreateInfo &wCreateInfo) {
    switch (RType) {
        case RE_OPENGL: {
            OglRender oglRender{};
            oglRender.setupAdapter(window, windowWidth, windowHeight);
            pGlfwWindow = oglRender.GetGlfwWindow();
            setupInput();


            while (!glfwWindowShouldClose(pGlfwWindow)) {
                Vector4 clear_color = Vector4(clearColor.x, clearColor.y,
                                            clearColor.z, clearColor.w);
                glfwPollEvents();

                // Initiate frame
                // Render frame

                glfwGetFramebufferSize(pGlfwWindow, &frameBufferSize[0], &frameBufferSize[1]);
                glViewport(0, 0, frameBufferSize[0], frameBufferSize[1]);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                             clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);

                // Render draw data
                glfwSwapBuffers(pGlfwWindow);
            }

            // Cleanup

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();

            oglRender.finish();
        }
        break;
        case RE_VULKAN: {
            VkRender vkRenderer{};
            vkRenderer.setupAdapter();
            vkRenderer.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = vkRenderer.GetGlfwWindow();
            setupInput();

            if (vkRenderer.init(pGlfwWindow) == EXIT_FAILURE) {
                throw std::runtime_error("Failed to init glfw");
            }

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
                vkRenderer.Draw();
            }

            vkRenderer.cleanup();
        }
        break;

        case RE_METAL: {
            MtlRender mtlRenderer{};
            mtlRenderer.initWindow(window, windowWidth, windowHeight, wCreateInfo);
            pGlfwWindow = mtlRenderer.GetGlfwWindow();
            setupInput();

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();

                @autoreleasepool {
                    glfwGetFramebufferSize(pGlfwWindow, &frameBufferSize[0], &frameBufferSize[1]);
                    mtlRenderer.layer.drawableSize = CGSizeMake(frameBufferSize[0], frameBufferSize[1]);
                    id <CAMetalDrawable> drawable = [mtlRenderer.layer nextDrawable];

                    id <MTLCommandBuffer> commandBuffer = [mtlRenderer.commandQueue commandBuffer];

                    mtlRenderer.renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
                            clearColor.x, clearColor.y,
                            clearColor.z, clearColor.w);;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:mtlRenderer.renderPassDescriptor];
                    [renderEncoder pushDebugGroup:@"Roast Metal"];

                    // Start frame
                    // Render frame

                    [renderEncoder popDebugGroup];
                    [renderEncoder endEncoding];

                    [commandBuffer presentDrawable:drawable];
                    [commandBuffer commit];
                }
            }

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();
        }
        break;

        case RE_NONE:
            break;
    }

}
