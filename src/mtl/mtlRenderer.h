#ifndef ROASTENGINE_MTLRENDERER_H
#define ROASTENGINE_MTLRENDERER_H

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>


class mtlRenderer {
private:
    GLFWwindow *pGlfwWindow;
    NSWindow *pNSWindow;
    CAMetalLayer *pSwapchain;

    id<MTLCommandQueue> queue;
    id<MTLDevice> gpu;


    NSWindow *getCocoaWindow(GLFWwindow *window) const;

    void SetupNsWindow(CAMetalLayer *swapchain, NSWindow *nswindow) const;

    void setupSwapchain();

    static void (*keyCallback)(int, int, int, int);

public:
    int run();

    GLFWwindow *createWindow(const int width, const int height, char *wName) const;


    int setupDevice();

    void init(GLFWwindow *newWindow);

    void cleanup();
};


#endif //ROASTENGINE_MTLRENDERER_H
