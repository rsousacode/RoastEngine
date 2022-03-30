#ifndef ROASTENGINE_MTLRENDER_H
#define ROASTENGINE_MTLRENDER_H

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import "../RDDefs.h"


struct MtlRender  {


    [[nodiscard]] GLFWwindow *GetGlfwWindow() const;

    CAMetalLayer *layer;
    id <MTLCommandQueue> commandQueue;
    MTLRenderPassDescriptor *renderPassDescriptor;
    id <MTLDevice> device;

    NSWindow *nswindow;
    GLFWwindow *pGlfwWindow;


    void initWindow(const char *wTitle, int width, int height, WindowCreateInfo &wCreateInfo);

     void SetupNSWindow(WindowCreateInfo &wCreateInfo);
};


#endif //ROASTENGINE_MTLRENDER_H
