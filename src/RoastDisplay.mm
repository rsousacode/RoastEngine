#include "RoastDisplay.h"
#import "mtl/MtlRender.h"
#import "vk/VkRender.h"
#import "ogl/OglRender.h"

void keyCb (GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void
RoastDisplay::Init(const RDCreateInfo& info, const RDWindowCreateInfo &wCreateInfo) {
    switch (info.typeCompatibility.Type) {
        case RE_OPENGL: {
            OglRender oglRender{};
            oglRender.prepare(wCreateInfo, info , keyCb);
            oglRender.prepareNextRender();
            pGlfwWindow = oglRender.pGlfwWindow;
            // Game loop
            while (!glfwWindowShouldClose(pGlfwWindow))
            {
                // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
                glfwPollEvents();

                // Render
                // Clear the colorbuffer
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                oglRender.draw();
                // Swap the screen buffers
                glfwSwapBuffers(pGlfwWindow);
            }

            // Terminates GLFW, clearing any resources allocated by GLFW.
            glfwTerminate();

        }
            break;
        case RE_VULKAN: {
            VkRender vkRenderer{};
            vkRenderer.setupAdapter();
            vkRenderer.initWindow(info.windowTitle, info.windowWidth, info.windowHeight);
            pGlfwWindow = vkRenderer.GetGlfwWindow();

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
            mtlRenderer.initWindow(info.windowTitle, info.windowWidth, info.windowHeight, wCreateInfo);
            pGlfwWindow = mtlRenderer.GetGlfwWindow();

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

