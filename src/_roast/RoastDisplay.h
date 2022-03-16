
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

public:
    int start(const char *windowTitle);

    explicit RoastDisplay(RE_Type rType);
};


#endif //ROASTENGINE_ROASTDISPLAY_H
