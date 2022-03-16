
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include "../vk/vkRenderer.h"

typedef enum RE_Type {
    RE_VULKAN = 0x00000001,
    RE_OPENGL = 0x00000002,
    RE_METAL = 0x00000003
} RE_RenderEngine;


#define WIDTH 1920
#define HEIGHT 1080

class RoastDisplay {

private:

    GLFWwindow *pGlfwWindow;
    RE_Type RType;

    // Glfw
    static void initGlfw();

    static void setupVkWindowHints();

    static void setupOglWindowHints();

    GLFWwindow * createGlfwWindow(const char *wName, int width, int height);

    // Vulkan
    GLFWwindow * initVkWindow(const char *wName, int width, int height);


    // OpenGL Code
    GLFWwindow * initOglWindow(const char *wName, int width, int height);

    int renderLoopOgl();

    int renderLoopVk();


    int renderLoopMetal();


    // Glew
    int initGlew();

    // GLFW misc
    static bool vulkanSupported();

    void hideWindow();

    void showWindow();

    int getWindowSize();

public:
    int start(const char *windowTitle);

    RoastDisplay(const RE_Type rType);
    static void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    GLFWwindow * initMetalWindow(const char *wTitle, int width, int height);

    void onGlfwKeyCallback(int key, int scancode, int action, int mods);

    void setupMetalWindowHints();
};


#endif //ROASTENGINE_ROASTDISPLAY_H
