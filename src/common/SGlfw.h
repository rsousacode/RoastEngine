#ifndef ROASTENGINE_SGLFW_H
#define ROASTENGINE_SGLFW_H

#import <GLFW/glfw3.h>

namespace SGlfw {

    /*!
	Use generic GLFW Hints
    */

    inline void
    UseEngineGlfwWindowHints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, 0);
        glfwWindowHint(GLFW_RESIZABLE, 1);
    }
}

#endif //ROASTENGINE_SGLFW_H
