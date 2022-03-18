#import <stdexcept>
#include "MtlRender.h"


bool MtlRender::setupAdapter() {
    gpu = MTLCreateSystemDefaultDevice();
    queue = [gpu newCommandQueue];
    swapchain = [CAMetalLayer layer];
    swapchain.device = gpu;
    swapchain.opaque = YES;
    return true;
}

bool MtlRender::initWindow(const char *wTitle, int width, int height) {
    if (glfwInit() == GLFW_FALSE) {
        return false;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pGlfwWindow= glfwCreateWindow(width, height, wTitle, NULL, NULL);
    nswindow = glfwGetCocoaWindow(pGlfwWindow);
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;
    return true;
}

GLFWwindow *MtlRender::GetGlfwWindow() {
    return pGlfwWindow;
}
