#import <GLFW/glfw3.h>
#import "OpenGL/gl.h"
#include "../RDDefs.h"

#ifndef ROASTENGINE_OGLRENDERER_H
#define ROASTENGINE_OGLRENDERER_H

struct OglRender {


    void draw();

    void prepare(const RDWindowCreateInfo &pInfo, const RDCreateInfo &createInfo, GLFWkeyfun keyCb);

    void prepareNextRender();

    GLFWwindow *pGlfwWindow;
};

#endif //ROASTENGINE_OGLRENDERER_H
