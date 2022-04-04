#include "MtlRender.h"

#import <stdexcept>
#import "../common/RDDebug.h"
#import "../common/SGlfw.h"

 void
MtlRender::initWindow(const char *wTitle, int width, int height, const RDWindowCreateInfo &wCreateInfo) {
    // Initialize System

    glfwSetErrorCallback(RDDebug::glfwErrorCallback);

    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to init glfw");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    SGlfw::UseEngineGlfwWindowHints();

    pGlfwWindow= glfwCreateWindow(width, height, wTitle, NULL, NULL);

    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    // Init Render options

    nswindow = glfwGetCocoaWindow(pGlfwWindow);
    SetupNSWindow(wCreateInfo);
    layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;


    renderPassDescriptor = [MTLRenderPassDescriptor new];

}

inline void
MtlRender::SetupNSWindow(const RDWindowCreateInfo &wCreateInfo) {
    nswindow.contentView.layer = layer;
    nswindow.titlebarAppearsTransparent = wCreateInfo.transparentTitleBar;
    if(wCreateInfo.showTitleBar) {
        nswindow.titleVisibility = NSWindowTitleVisible ;
    } else {
        nswindow.titleVisibility = NSWindowTitleHidden;
    }

    if(wCreateInfo.fullContentView) {
        nswindow.titlebarAppearsTransparent = true;
        nswindow.titleVisibility = NSWindowTitleHidden;
        nswindow.styleMask += NSWindowStyleMaskFullSizeContentView;
    }
    nswindow.alphaValue = wCreateInfo.alphaValue;
    nswindow.contentView.wantsLayer = YES;
}

GLFWwindow
*MtlRender::GetGlfwWindow() const {
    return pGlfwWindow;
}
