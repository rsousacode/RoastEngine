#include <GLFW/glfw3.h>
#include "OglRender.h"
#include <stdexcept>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void OglRender::setupAdapter(const char *wName, int wWidth, int wHeight) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto initResult = glfwInit();
    if (initResult == GLFW_FALSE) {
        throw std::runtime_error("GLFW Failed to setupCocoa");
    }
    glfwSetErrorCallback(error_callback);
    glfwWindow = glfwCreateWindow(wWidth, wHeight, wName, nullptr, nullptr);
    glfwMakeContextCurrent(glfwWindow);

}

GLFWwindow *OglRender::GetGlfwWindow() {
    return glfwWindow;
}
