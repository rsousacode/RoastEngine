#include "RoastDisplay.h"
#include "../mtl/mtlRenderer.h"

RoastDisplay::RoastDisplay(const RE_Type rType) {
    RType = rType;
}

void
RoastDisplay::initGlfw() {
    auto initResult = glfwInit();
    if(initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to init");
    }
}

void
RoastDisplay::setupVkWindowHints() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

GLFWwindow *
RoastDisplay::createGlfwWindow(const char *wName, int width, int height) {
    pGlfwWindow = glfwCreateWindow(width, height, wName, nullptr, nullptr);
}

GLFWwindow *
RoastDisplay::initMetalWindow(const char *wTitle, int width, int height) {
    initGlfw();
    setupMetalWindowHints();
}

void
RoastDisplay::setupMetalWindowHints() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}


GLFWwindow *
RoastDisplay::initVkWindow(const char *wName, const int width, const int height) {
    initGlfw();
    setupVkWindowHints();
    return createGlfwWindow(wName, width, height);
}



void
RoastDisplay::setupOglWindowHints() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow *
RoastDisplay::initOglWindow(const char *wName, const int width, const int height) {
/*    initGlfw();
    setupOglWindowHints();
    createGlfwWindow(wName, width, height);
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(pGlfwWindow, &bufferWidth, &bufferHeight);

    // Set context for glew
    glfwMakeContextCurrent(pGlfwWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    initGlew();

    glViewport(0, 0, bufferHeight, bufferHeight);*/
}

int
RoastDisplay::renderLoopMetal() {

    mtlRenderer metalRenderer;
    metalRenderer.init(pGlfwWindow);

    glfwSetKeyCallback(pGlfwWindow, onKeyCallback);

    while (!glfwWindowShouldClose(pGlfwWindow)) {
        glfwPollEvents();
    }

    metalRenderer.cleanup();

    return 0;

    return 0;
}


int
RoastDisplay::renderLoopVk() {

    vkRenderer vkRenderer;

    auto initResult = vkRenderer.init(pGlfwWindow);

    if (initResult == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(pGlfwWindow)) {
        glfwPollEvents();
    }

    vkRenderer.cleanup();

    return 0;
}


int
RoastDisplay::renderLoopOgl() {

/*    while(!glfwWindowShouldClose(pGlfwWindow)) {
        glfwPollEvents();

        // Clear Window
    }*/
    return 0;
}

void
RoastDisplay::onGlfwKeyCallback(int key, int scancode, int action, int mods) {

}



int
RoastDisplay::start(const char *windowTitle) {
    switch (RType) {
        case RE_OPENGL:
            pGlfwWindow =initOglWindow(windowTitle, WIDTH, HEIGHT);
            return renderLoopOgl();

        case RE_VULKAN:
            #define GLFW_INCLUDE_VULKAN
            pGlfwWindow = initVkWindow(windowTitle, WIDTH, HEIGHT);
            return renderLoopVk();

        case RE_METAL:
            pGlfwWindow = initMetalWindow(windowTitle, WIDTH, HEIGHT);
            return renderLoopMetal();
    }
}

void
RoastDisplay::hideWindow() {
    glfwHideWindow(pGlfwWindow);
}

void
RoastDisplay::showWindow() {
    glfwShowWindow(pGlfwWindow);
}

bool
RoastDisplay::vulkanSupported() {
    return glfwVulkanSupported() == GLFW_TRUE;
}

int
RoastDisplay::getWindowSize() {
    return 0;
}

int
RoastDisplay::initGlew() {
/*    if(!glewInit()) {
        printf("GLEW Init failed");
        glfwDestroyWindow(pGlfwWindow);
        glfwTerminate();
        return EXIT_FAILURE;
    }*/
    return 0;
}

void RoastDisplay::onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

