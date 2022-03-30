#include "RoastDisplay.h"
#include "RDDefs.h"
#include "RMath.h"

int main() {

    SetupCreateInfo createInfo{
            .typeCompatibility      =   { .Type = RE_METAL },
            .windowTitle        =    "Roast Engine",
            .majorVersion       =    1,
            .minorVersion       =    0,
            .branch             =    "",
            .windowWidth        =    1500,
            .windowHeight       =    900,
    };

    WindowCreateInfo windowCreateInfo = {
            .showTitleBar = true,
            .transparentTitleBar = false,
            .fullContentView = true
    };

    RoastDisplay display;
    display.Init(createInfo, windowCreateInfo);

    return 0;
}