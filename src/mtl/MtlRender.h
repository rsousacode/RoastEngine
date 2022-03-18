#ifndef ROASTENGINE_MTLRENDER_H
#define ROASTENGINE_MTLRENDER_H

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>


class MtlRender  {

public:
    bool initWindow(const char *wTitle, int width, int height);
    bool setupAdapter();
    GLFWwindow *GetGlfwWindow();

    CAMetalLayer *swapchain;
    id<MTLCommandQueue> queue;

private:
    id <MTLDevice> gpu;
    NSWindow *nswindow;
    GLFWwindow *pGlfwWindow;

};


#endif //ROASTENGINE_MTLRENDER_H
