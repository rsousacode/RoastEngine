
#ifndef ROASTENGINE_RERENDERER_H
#define ROASTENGINE_RERENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include "vk/vkRenderer.h"


typedef enum RE_RenderEngine {
    RE_VULKAN = 0x00000001,
    RE_OPENGL = 0x00000002,
    RE_METAL  = 0x00000003
} RE_RenderEngine;


class RERenderer {

private:
    GLFWwindow *rWindow;

    // Glfw and dependencies init
    static void           initGlfw();
    void                  createGlfwWindow(const std::string &wName, const int width, const int height);
    static void           setupVkWindowHints();
    static void           setupOglWindowHints();

    // Glew
    int initGlew();

    // Vulkan code
    void    initVkWindow(const std::string& wName , const int width, const int height);
    void    initOglWindow(const std::string &wName, const int width, const int height);
    int     renderLoopVk();

    // OpenGL Code
    int     renderLoopOgl();

    // GLFW misc
    static bool    vulkanSupported();
    void    hideWindow();
    void    showWindow();

    int     getWindowSize();

public:
    int     Init(RE_RenderEngine &renderType);
};


#endif //ROASTENGINE_RERENDERER_H
