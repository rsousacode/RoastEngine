#include "_roast/RoastDisplay.h"


int main() {

    RoastDisplay renderer{
        RE_VULKAN
    };

    renderer.start("Roast Engine");

    return 0;
}