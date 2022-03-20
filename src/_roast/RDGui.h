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
    ShowRInfo(RDImGuiState *imGuiState, RDType rdType) {

        ImGui::Begin("Render Info", &imGuiState->showRinfo);                          // Create a window called "Hello, world!" and append into it.

        switch (rdType) {

            case RE_VULKAN:
                ImGui::Text("VULKAN - ImGUI");
                break;
            case RE_OPENGL:
                ImGui::Text("OPENGL- ImGUI");
                break;
            case RE_METAL:
                ImGui::Text("METAL- ImGUI");
                break;
        }

        ImGui::ColorEdit3("Clear color", (float*)&imGuiState->clearColor); // Edit 3 floats representing a color

        ImGui::Spacing();
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        ImGui::End();
    }


    inline void
    ShowDemo() {
        ImGui::ShowDemoWindow(reinterpret_cast<bool *>(true));
    }


}
