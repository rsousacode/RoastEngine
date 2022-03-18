#include "RoastDisplay.h"

#include "../mtl/MtlRender.h"
#include "../vk/VkRender.h"
#import "../ogl/OglRender.h"

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
RoastDisplay::createRenderer(const RDCreateInfo& info) {
    RType = info.typeCompatibility.Type;
    windowWidth = info.windowWidth;
    windowHeight = info.windowHeight;
    start(info.windowTitle);
}

int
RoastDisplay::start(const char *window) {
    VkRender vkRenderer;
    MtlRender mtlRenderer{};
    OglRender oglRender {};

    switch (RType) {
        case RE_OPENGL:
            oglRender.setupAdapter();
            oglRender.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = oglRender.GetGlfwWindow();
            setupInput();

            while(!glfwWindowShouldClose(pGlfwWindow)) {
                //glClear(GL_COLOR_BUFFER_BIT);

                //glfwSwapBuffers(pGlfwWindow);

                glBegin(GL_QUADS);

                    glColor3f(1,0,0);
                    glVertex3f(-0.5f, 0.5f, 0.0f);

                    glColor3f (0, 1, 0);
                    glVertex3f(0.5f, .5f, .0f);

                    glColor3f(.0, .0, 1.);
                    glVertex3f(0.5f, -0.5f, 0.0f);

                    glColor3f(.0,.0,1.);
                    glVertex3f(-.5f, -.5f, .0f);

                glEnd();

                glfwPollEvents();
            }

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();

            return finish();

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

            return finish();

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

            return finish();
    }

    return 0;
}

int
RoastDisplay::finish() {
    return 0;
}

RoastDisplay
*RoastDisplay::Instance() {

    static auto* display = new RoastDisplay;
    return display;
}
