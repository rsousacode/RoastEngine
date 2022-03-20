#include "imgui.h"
#include "RDDefs.h"

namespace RDGui {
    inline void
    ShowAnotherWindow(RDImGuiState *imGuiState) {
        ImGui::Begin("Another Window", &imGuiState->showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            imGuiState->showAnotherWindow = false;
        ImGui::End();
    }

    inline void
    ShowDemo() {
        ImGui::ShowDemoWindow(reinterpret_cast<bool *>(true));
    }


}
