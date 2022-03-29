#include "RoastDisplay.h"
#include "RDDefs.h"
#include "RMath.h"

int main() {

    RDSetupCreateInfo createInfo{
            .typeCompatibility      =   { .Type = RE_OPENGL },
            .windowTitle        =    "Roast Engine",
            .majorVersion       =    1,
            .minorVersion       =    0,
            .branch             =    "",
            .windowWidth        =    1500,
            .windowHeight       =    900,
    };

    RoastDisplay display;
    display.Init(createInfo);

    return 0;
}