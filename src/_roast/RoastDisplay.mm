#include "RoastDisplay.h"

#include "../mtl/MtlRender.h"
#include "../vk/VkRender.h"
#import "../ogl/OglRender.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#import "imgui_impl_opengl3.h"

void
RoastDisplay::handleInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void
RoastDisplay::setupInput() {
    if (shouldHandleInput) {
        glfwSetKeyCallback(pGlfwWindow, handleInput);
    }
}

void
RoastDisplay::createRenderer(const RDCreateInfo& info) {
    RType = info.typeCompatibility.Type;
    windowWidth = info.windowWidth;
    windowHeight = info.windowHeight;
    start(info.windowTitle);
}

int
RoastDisplay::start(const char *window) {
    VkRender vkRenderer{};
    MtlRender mtlRenderer{};
    OglRender oglRender{};

    switch (RType) {
        case RE_OPENGL:
            oglRender.setupAdapter(window, windowWidth, windowHeight);
            pGlfwWindow = oglRender.GetGlfwWindow();
            setupInput();


            while(!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                RenderGUI();
                ImVec4 clear_color = ImVec4(imGuiState.clearColor[0], imGuiState.clearColor[1], imGuiState.clearColor[2], imGuiState.clearColor[3]);
                // Rendering
                ImGui::Render();
                glfwGetFramebufferSize(pGlfwWindow, &frameBufferSize[0], &frameBufferSize[1]);
                glViewport(0, 0, frameBufferSize[0], frameBufferSize[1]);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(pGlfwWindow);
            }

            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();

            return oglRender.finish();
        case RE_VULKAN:
            vkRenderer.setupAdapter();
            vkRenderer.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = vkRenderer.GetGlfwWindow();
            setupInput();

            if (vkRenderer.init(pGlfwWindow) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();
            }

            vkRenderer.cleanup();

            return 0;

        case RE_METAL:
            mtlRenderer.initWindow(window, windowWidth, windowHeight);
            pGlfwWindow = mtlRenderer.GetGlfwWindow();
            setupInput();

            if (!ImGui_ImplMetal_Init(mtlRenderer.device)) {
                throw std::runtime_error("Failed to initialize metal");
            }

            while (!glfwWindowShouldClose(pGlfwWindow)) {
                glfwPollEvents();

                @autoreleasepool {
                    glfwGetFramebufferSize(pGlfwWindow, &frameBufferSize[0], &frameBufferSize[1]);
                    mtlRenderer.layer.drawableSize = CGSizeMake(frameBufferSize[0], frameBufferSize[1]);
                    id<CAMetalDrawable> drawable = [mtlRenderer.layer nextDrawable];

                    id<MTLCommandBuffer> commandBuffer = [mtlRenderer.commandQueue commandBuffer];

                    mtlRenderer.renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(imGuiState.clearColor[0], imGuiState.clearColor[1],
                                                                                                        imGuiState.clearColor[2], imGuiState.clearColor[3]);;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:mtlRenderer.renderPassDescriptor];
                    [renderEncoder pushDebugGroup:@"ImGui demo"];

                    // Start the Dear ImGui frame
                    ImGui_ImplMetal_NewFrame(mtlRenderer.renderPassDescriptor);
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                    RenderGUI();

                    // Rendering
                    ImGui::Render();
                    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

                    [renderEncoder popDebugGroup];
                    [renderEncoder endEncoding];

                    [commandBuffer presentDrawable:drawable];
                    [commandBuffer commit];
                }
            }

            glfwDestroyWindow(pGlfwWindow);
            glfwTerminate();
            return 0;
    }

    return 0;
}

inline void
RoastDisplay::RenderGUI() {

    //CreateMenuBar();

    ShowOverlay();

    if(imGuiState.showDemoWindow) {
        ImGui::ShowDemoWindow(&imGuiState.showDemoWindow);
    }

    if(imGuiState.showRenderInfo) {
        ShowRInfo( RType);

    }

}

inline void
RoastDisplay::ShowRInfo(RDType rdType) {

    ImGui::Begin("Render Info", &imGuiState.showRenderInfo, ImGuiWindowFlags_NoCollapse);
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background


    ImGui::ColorEdit3("Clear color", (float*)&imGuiState.clearColor); // Edit 3 floats representing a color

    ImGui::Spacing();

    ImGui::Text("Frame buffer width %d", frameBufferSize[0]);
    ImGui::Text("Frame buffer height %d", frameBufferSize[1]);


    ImGui::End();
}


inline void
RoastDisplay::ShowConsole() {

}

inline void
RoastDisplay::ShowOverlay() {
    static int corner = 2;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", &imGuiState.showOverlay, window_flags))
    {

        switch (RType) {

            case RE_VULKAN:
                ImGui::Text("ROAST [VULKAN]");
                break;
            case RE_OPENGL:
                ImGui::Text("Roast- [OPEN GL]");
                break;
            case RE_METAL:
                ImGui::Text("Roast [METAL]");
                break;
        }
        ImGui::Separator();

        ImGui::Text("IMGUI FPS: %.1f", ImGui::GetIO().Framerate);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (imGuiState.showOverlay && ImGui::MenuItem("Close")) *&imGuiState.showOverlay = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

inline void
RoastDisplay::CreateMenuBar() {

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
            }

            if (ImGui::MenuItem("New Project"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Console"))
            {
                ShowConsole();
            }

            if (ImGui::MenuItem("Render Info"))
            {
                ShowRinfo();
            }

            if (ImGui::MenuItem("Show Demo"))
            {
                imGuiState.showDemoWindow = !imGuiState.showDemoWindow;
            }

            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}

void RoastDisplay::ShowRinfo() {
    imGuiState.showRenderInfo = !imGuiState.showRenderInfo;
}
