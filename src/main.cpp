#include "_roast/RoastDisplay.h"


int main() {

    RoastDisplay renderer{
        RE_METAL
    };

    renderer.start("Roast Engine");

    return 0;
}