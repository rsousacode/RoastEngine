#include "_roast/RoastDisplay.h"
#include "_roast/RDDefs.h"

int main() {

    RDCreateInfo createInfo{
            .displayEngine      =   RE_OPENGL,
            .windowTitle        =    "Roast Engine",
            .majorVersion       =    1,
            .minorVersion       =    0,
            .branch             =    "",
            .windowWidth        =    1080,
            .windowHeight       =    610,
    };



    RoastDisplay display;
    display.createRenderer(createInfo);

    return 0;
}