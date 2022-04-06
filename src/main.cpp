#include "RoastDisplay.h"
#include "RDDefs.h"
#include "RMath.h"
#include "engine/RInit.h"

int
main() {

    SetupCreateInfo createInfo = RInit::GetEngineSetupCreateInfo();

    RDWindowCreateInfo windowCreateInfo = {};

    RoastDisplay display;

    display.Init(createInfo, windowCreateInfo);

    return 0;
}
