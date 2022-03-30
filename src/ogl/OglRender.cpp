#include "OglRender.h"
#import <GLFW/glfw3.h>
#import "../common/RDDebug.h"
#import<stdexcept>

void OglRender::setupAdapter(const char *wName, int wWidth, int wHeight) {
    glfwInit();
    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    glfwSetErrorCallback(RDDebug::glfwErrorCallback);

    auto initResult = glfwInit();
    if (initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to setupCocoa");
    }
    glfwWindow = glfwCreateWindow(wWidth, wHeight, wName, nullptr, nullptr);
    glfwMakeContextCurrent(glfwWindow);

    // Create Render context


}

GLFWwindow *OglRender::GetGlfwWindow() {
    return glfwWindow;
}

int
OglRender::finish() {
    return 0;
}
