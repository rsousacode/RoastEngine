#include "RERenderer.h"
#include "mtl/mtlRenderer.h"

void
RERenderer::initGlfw() {
    auto initResult = glfwInit();
    if(initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to init");
    }
}

void
RERenderer::setupVkWindowHints() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void
RERenderer::createGlfwWindow(const std::string &wName, int width, int height) {
    rWindow = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}


void
RERenderer::initVkWindow(const std::string &wName, const int width, const int height) {

    initGlfw();
    setupVkWindowHints();
    createGlfwWindow(wName, width, height);
}

void
RERenderer::setupOglWindowHints() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void
RERenderer::initOglWindow(const std::string &wName, const int width, const int height) {
/*    initGlfw();
    setupOglWindowHints();
    createGlfwWindow(wName, width, height);
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(rWindow, &bufferWidth, &bufferHeight);

    // Set context for glew
    glfwMakeContextCurrent(rWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    initGlew();

    glViewport(0, 0, bufferHeight, bufferHeight);*/
}


int
RERenderer::renderLoopVk() {

    vkRenderer vkRenderer;

    auto initResult = vkRenderer.init(rWindow);

    if (initResult == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(rWindow)) {
        glfwPollEvents();
    }

    vkRenderer.cleanup();

    return 0;
}


int
RERenderer::renderLoopOgl() {

/*    while(!glfwWindowShouldClose(rWindow)) {
        glfwPollEvents();

        // Clear Window
    }*/
    return 0;
}



int
RERenderer::Init(RE_RenderEngine &renderType) {
    switch (renderType) {
        case RE_OPENGL:
            initOglWindow("OpenGL Window", 800, 600);
            return renderLoopOgl();

        case RE_VULKAN:
            #define GLFW_INCLUDE_VULKAN
            initVkWindow("Vulkan window", 800, 600);
            return renderLoopVk();

        case RE_METAL:
            mtlRenderer metalRenderer;
            return metalRenderer.run();
    }
}

void
RERenderer::hideWindow() {
    glfwHideWindow(rWindow);
}

void
RERenderer::showWindow() {
    glfwShowWindow(rWindow);
}

bool
RERenderer::vulkanSupported() {
    return glfwVulkanSupported() == GLFW_TRUE;
}

int
RERenderer::getWindowSize() {
    return 0;
}

int
RERenderer::initGlew() {
/*    if(!glewInit()) {
        printf("GLEW Init failed");
        glfwDestroyWindow(rWindow);
        glfwTerminate();
        return EXIT_FAILURE;
    }*/
    return 0;
}




