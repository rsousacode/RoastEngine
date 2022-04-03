
#ifndef ROASTENGINE_ROASTDISPLAY_H
#define ROASTENGINE_ROASTDISPLAY_H

#include <GLFW/glfw3.h>

#import "RDDefs.h"
#import "RMath.h"


/*!
	Responsible to manage essential information related to the display process.
*/

struct RoastDisplay {

    GLFWwindow *pGlfwWindow{};

    Vector4 clearColor = Vector4(0.2, 0.3, 0.4, 1.0);

    int frameBufferSize[2]{};

    void Init(const RDCreateInfo& rci, const RDWindowCreateInfo &wci);

};


#endif //ROASTENGINE_ROASTDISPLAY_H
