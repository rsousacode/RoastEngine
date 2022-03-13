#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include "vk_renderer/vkRenderer.h"

vkRenderer vKRendererImplm;

GLFWwindow *rWindow;

void initWindow(const std::string& wName = "Roast Test", const int width = 800, const int height = 600) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    rWindow = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

int main() {

    initWindow("Roast Engine", 800, 600);

    auto initResult = vKRendererImplm.init(rWindow);

    if (initResult == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Loop until closed

    while (!glfwWindowShouldClose(rWindow)) {
        glfwPollEvents();
    }

    vKRendererImplm.cleanup();

    return 0;
}