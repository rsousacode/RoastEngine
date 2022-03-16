#include "mtlRenderer.h"

int mtlRenderer::setupDevice() {
    gpu = MTLCreateSystemDefaultDevice();
    queue = [gpu newCommandQueue];
    return 0;
}

GLFWwindow *mtlRenderer::setupCocoa(GLFWwindow *newWindow) {
    const id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
    const id<MTLCommandQueue> queue = [gpu newCommandQueue];
    CAMetalLayer *swapchain = [CAMetalLayer layer];
    swapchain.device = gpu;
    swapchain.opaque = YES;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = newWindow;
    NSWindow *nswindow = getCocoaWindow(window);
    SetupNsWindow(swapchain, nswindow);
    return window;

}

void mtlRenderer::setupSwapchain() {
    pSwapchain = [CAMetalLayer layer];
    pSwapchain.device = gpu;
    pSwapchain.opaque = YES;
}

void mtlRenderer::SetupNsWindow(CAMetalLayer *swapchain, NSWindow *nswindow) const {
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;
}

NSWindow *mtlRenderer::getCocoaWindow(GLFWwindow *window) const {
    return glfwGetCocoaWindow(window);
}

void mtlRenderer::cleanup() {

}


