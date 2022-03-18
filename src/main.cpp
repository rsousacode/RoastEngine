#include "_roast/RoastDisplay.h"
#include "_roast/RDDefs.h"

int main() {

    RDCreateInfo createInfo{
            .displayEngine      =   RE_VULKAN,
            .windowTitle         =    "Roast Engine",
            .majorVersion       =    1,
            .minorVersion       =    0,
            .branch             =    "",
            .windowWidth        =    1080,
            .windowHeight       =    610,
    };

    RoastDisplay display;

    if (display.createRenderer(createInfo) == RD_FAILURE) {
        throw std::runtime_error("Failed to init Roast Display");
    }

    return 0;
}