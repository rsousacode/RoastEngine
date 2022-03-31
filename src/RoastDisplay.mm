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

            auto r = glfwInit();
            if(r == GLFW_FALSE) {
                throw std::runtime_error("Failed to init GLFW");
            }
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

            // return a window
            // also create a OpenGL context associated with this window
            pGlfwWindow = glfwCreateWindow(600, 600, "my triangle app", nullptr, nullptr);
            // make the context of this window current for main thread
            glfwMakeContextCurrent(pGlfwWindow);

            oglRender.setupAdapter(window, windowWidth, windowHeight);

            while (!glfwWindowShouldClose(pGlfwWindow))
            {

                glfwPollEvents();

                // Initiate frame
                // Render frame
                glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w,
                             clearColor.z * clearColor.w, clearColor.w);
                glClear(GL_COLOR_BUFFER_BIT);
                // display
                oglRender.draw();

                // swap the front and back buffers
                glfwSwapBuffers(pGlfwWindow);
                // poll all pending events and process
                glfwPollEvents();
            }

            //glDeleteVertexArrays(NumVAOs, VAOs);
            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();



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
