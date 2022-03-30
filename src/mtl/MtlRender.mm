#import <stdexcept>
#include "MtlRender.h"
#import "../common/RDDebug.h"


bool MtlRender::initWindow(const char *wTitle, int width, int height) {
    // Initialize System

    glfwSetErrorCallback(RDDebug::glfwErrorCallback);

    if (glfwInit() == GLFW_FALSE) {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pGlfwWindow= glfwCreateWindow(width, height, wTitle, NULL, NULL);

    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    // Init Render options

    nswindow = glfwGetCocoaWindow(pGlfwWindow);
    layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswindow.contentView.layer = layer;
    nswindow.contentView.wantsLayer = YES;

    renderPassDescriptor = [MTLRenderPassDescriptor new];

    return true;
}

GLFWwindow *MtlRender::GetGlfwWindow() {
    return pGlfwWindow;
}
