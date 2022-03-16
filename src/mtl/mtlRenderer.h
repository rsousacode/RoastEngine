#ifndef ROASTENGINE_MTLRENDERER_H
#define ROASTENGINE_MTLRENDERER_H

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>


class mtlRenderer  {
private:
    GLFWwindow *pGlfwWindow{};
    NSWindow *pNSWindow{};
    CAMetalLayer *pSwapchain{};

    id<MTLCommandQueue> queue{};

    id<MTLDevice> gpu{};


    NSWindow *getCocoaWindow(GLFWwindow *window) const;

    void SetupNsWindow(CAMetalLayer *swapchain, NSWindow *nswindow) const;

    void setupSwapchain();

public:

    GLFWwindow *setupCocoa(GLFWwindow *newWindow);

    int setupDevice();

    void cleanup();
};


#endif //ROASTENGINE_MTLRENDERER_H
