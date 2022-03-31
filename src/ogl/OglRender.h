#import <GLFW/glfw3.h>
#import "OpenGL/gl.h"

#ifndef ROASTENGINE_OGLRENDERER_H
#define ROASTENGINE_OGLRENDERER_H


struct OglRender {


    void setupAdapter(const char *wName, int wWidth, int wHeight);

    void draw();

    void preSetup();
};

#endif //ROASTENGINE_OGLRENDERER_H
