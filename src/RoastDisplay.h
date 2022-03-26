
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#import<stdexcept>
#import <iostream>

#import "RDDefs.h"

struct RoastDisplay {

    int windowWidth, windowHeight;

    GLFWwindow *pGlfwWindow;

    RDType RType;

    RDImGuiState imGuiState;

    int start(const char *window);

    int frameBufferSize[2];

    bool shouldHandleInput = true;

    static void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods);

    void setupInput();

    inline void RenderGUI();

    inline void CreateMenuBar();

    inline void ShowConsole();

    inline void ShowRinfo();

    inline void ShowRInfo(RDType rdType);

    inline void ShowOverlay();

    void createRenderer(const RDCreateInfo& info);

};


#endif //ROASTENGINE_ROASTDISPLAY_H
