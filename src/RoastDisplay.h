
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GLFW/glfw3.h>

#import "RDDefs.h"
#import "RMath.h"

struct RoastDisplay {


    GLFWwindow *pGlfwWindow;
    Vector4 clearColor = Vector4(1.0, 0.0, 0.0, 1.0);

    int frameBufferSize[2];
    bool shouldHandleInput = true;

    void Init(const RDCreateInfo& info, const RDWindowCreateInfo &wCreateInfo);

};


#endif //ROASTENGINE_ROASTDISPLAY_H
