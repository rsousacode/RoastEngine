#ifndef ROASTENGINE_MTLRENDER_H
#define ROASTENGINE_MTLRENDER_H

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>


class MtlRender  {

public:
    bool initWindow(const char *wTitle, int width, int height);

    GLFWwindow *GetGlfwWindow();

    CAMetalLayer *layer;
    id <MTLCommandQueue> commandQueue;
    MTLRenderPassDescriptor *renderPassDescriptor;
    id <MTLDevice> device;
private:
    NSWindow *nswindow;
    GLFWwindow *pGlfwWindow;

    void Render();
};


#endif //ROASTENGINE_MTLRENDER_H
