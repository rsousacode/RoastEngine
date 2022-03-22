#include "_roast/RoastDisplay.h"
#include "_roast/RDDefs.h"

int main() {

    RDSetupCreateInfo createInfo{
            .typeCompatibility      =   { .Type = RE_METAL },
            .windowTitle        =    "Roast Engine",
            .majorVersion       =    1,
            .minorVersion       =    0,
            .branch             =    "",
            .windowWidth        =    1500,
            .windowHeight       =    900,
    };


    RoastDisplay display;
    display.createRenderer(createInfo);

    return 0;
}