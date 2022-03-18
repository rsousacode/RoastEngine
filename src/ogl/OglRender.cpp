#include <GLFW/glfw3.h>
#include "OglRender.h"
#include <stdexcept>

void OglRender::setupAdapter() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    auto initResult = glfwInit();
    if (initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to setupCocoa");
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void OglRender::initWindow(const char *wName, int wWidth, int wHeight) {
    glfwWindow = glfwCreateWindow(wWidth, wHeight, wName, nullptr, nullptr);
}

GLFWwindow *OglRender::GetGlfwWindow() {
    return glfwWindow;
}
