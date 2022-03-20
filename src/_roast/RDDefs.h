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
    [[maybe_unused]] int majorVersion = 1;
    [[maybe_unused]] int minorVersion = 0;
    [[maybe_unused]] const char *branch = "rc";
    int windowWidth = 1080;
    int windowHeight = 610;
} RDSetupCreateInfo;

typedef struct RDGuiState {
    bool showDemoWindow = true;
    bool showAnotherWindow = false;
    float clearColor[4]  = {0.45f, 0.55f, 0.60f, 1.00f};;
} RDImGuiState;
