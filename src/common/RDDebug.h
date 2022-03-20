#include <cstdio>

namespace RDDebug {

    inline void glfwErrorCallback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

}