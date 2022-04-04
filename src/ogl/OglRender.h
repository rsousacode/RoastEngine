#import <GLFW/glfw3.h>
#import "OpenGL/gl.h"
#include "../RDDefs.h"
#include "../RMath.h"

#ifndef ROASTENGINE_OGLRENDERER_H
#define ROASTENGINE_OGLRENDERER_H

struct OglRender {

    void display(Vector4 &glClearColor);

    void prepare(const RDWindowCreateInfo &pInfo, const RDCreateInfo &createInfo, GLFWkeyfun keyCb);

    void bindShaderStep();

    GLFWwindow *pGlfwWindow;
};

#endif //ROASTENGINE_OGLRENDERER_H
