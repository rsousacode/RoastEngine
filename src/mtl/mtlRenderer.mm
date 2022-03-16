#include "mtlRenderer.h"

int mtlRenderer::setupDevice() {
    gpu = MTLCreateSystemDefaultDevice();
    queue = [gpu newCommandQueue];

}

int mtlRenderer::run() {

    while (!glfwWindowShouldClose(pGlfwWindow)) {
        glfwPollEvents();

    }

    glfwDestroyWindow(pGlfwWindow);
    glfwTerminate();

    return 0;
}


void mtlRenderer::init(GLFWwindow *newWindow) {
    setupDevice();
    setupSwapchain();
    pGlfwWindow = newWindow;
    pNSWindow = getCocoaWindow(pGlfwWindow);
    SetupNsWindow(pSwapchain, pNSWindow);
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

GLFWwindow *mtlRenderer::createWindow(const int width, const int height, char *wName) const {
    GLFWwindow *window = glfwCreateWindow(width, height, wName, NULL, NULL);
    return window;
}

void mtlRenderer::cleanup() {

}


