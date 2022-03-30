
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#import<stdexcept>
#import <iostream>

#import "RDDefs.h"
#import "RMath.h"

struct RoastDisplay {


    GLFWwindow *pGlfwWindow;
    RDType RType;
    Vector4 clearColor = Vector4(1.0, 0.0, 0.0, 0.0);

    int windowWidth, windowHeight;
    int frameBufferSize[2];
    bool shouldHandleInput = true;

     void Init(const RDCreateInfo& info, WindowCreateInfo &wCreateInfo);
     void start(const char *window, WindowCreateInfo &wCreateInfo);
    inline void setupInput();
    static void handleInput(GLFWwindow *window, int key, int scancode, int action, int mods);




};


#endif //ROASTENGINE_ROASTDISPLAY_H
