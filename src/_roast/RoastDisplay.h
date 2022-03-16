
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#import "RDDefs.h"

class RoastDisplay {

private:

    int windowWidth, windowHeight;

    GLFWwindow *pGlfwWindow;

    RDType RType;

    // Glfw
    static void initGlfw();

public:
    int start(const char *window);

    bool shouldHandleInput = true;

    static void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods);

    void setupInput();

    RDResult createRenderer(const RDCreateInfo& info);
};


#endif //ROASTENGINE_ROASTDISPLAY_H
