
#include "re_renderer/RERenderer.h"

int main() {
    RE_RenderEngine renderType = RE_METAL;
    RERenderer renderer{};
    renderer.Init(renderType);
    return 0;
}