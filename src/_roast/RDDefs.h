typedef enum RDResult {
    RD_SUCCESS = 0,
    RD_FAILURE = 1,
} RDResult;

typedef enum RDType {
    RE_VULKAN = 0x00000001,
    RE_OPENGL = 0x00000002,
    RE_METAL = 0x00000003
} RE_RenderEngine;

typedef struct RDTypeComp {
    RDType Type;
} RDTypeCompatibility;

typedef struct RDCreateInfo {
    RDTypeCompatibility typeCompatibility{.Type = RE_VULKAN};
    const char *windowTitle = "Roast Engine";
    int majorVersion = 1;
    int minorVersion = 0;
    const char *branch = "rc";
    int windowWidth = 1080;
    int windowHeight = 610;
} RDSetupCreateInfo;
