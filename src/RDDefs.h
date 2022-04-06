#define BUFFER_OFFSET(a) ((void*)(a))
#include <string>


typedef enum RDResult {
    RD_SUCCESS = 0,
    RD_FAILURE = 1,
} RDResult;

typedef enum RDType {
    RE_VULKAN = 0x00000001,
    RE_OPENGL = 0x00000002,
    RE_METAL = 0x00000003,
    RE_NONE = 0x00000004
} RE_RenderEngine;

typedef struct RDTypeComp {
    RDType Type;
} RDTypeCompatibility;

typedef struct RDCreateInfo {
    RDTypeCompatibility typeCompatibility{.Type = RE_VULKAN};
    std::string windowTitle = "Roast Engine";
    int majorVersion = 1;
    int minorVersion = 0;
    std::string branch = "rc";
    int windowWidth = 1080;
    int windowHeight = 610;
} SetupCreateInfo;

struct RDWindowCreateInfo {
    // Shows or hides title bar on MacOS
    // in the future this option should apply to other OS's as well
    bool showTitleBar = true;
    bool transparentTitleBar = false;

    // Applies only on mac,
    // Means no title bar visible, only the close, minimize and maximize icons
    bool fullContentView = false;

    // Alpha value of the window itself
    float alphaValue = 1;

} ;

