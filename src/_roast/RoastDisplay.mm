#include "RoastDisplay.h"

#include "../mtl/MtlRender.h"
#include "../vk/VkRender.h"
#import "../ogl/OglRender.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#import "imgui_impl_opengl3.h"
#import "RDGui.h"

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
                    glfwGetFramebufferSize(pGlfwWindow, &windowWidth, &windowHeight);
                    mtlRenderer.layer.drawableSize = CGSizeMake(windowWidth, windowHeight);
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

    CreateMenuBar();

    if(imGuiState.showDemoWindow) {
        ImGui::ShowDemoWindow(&imGuiState.showDemoWindow);
    }

    if(imGuiState.showRinfo) {
        ShowRInfo( RType);

    }

    if (imGuiState.showAnotherWindow)
    {
        RDGui::ShowAnotherWindow(&imGuiState);
    }

    //RDGui::ShowDemo();

}

inline void
RoastDisplay::ShowRInfo(RDType rdType) {

    ImGui::Begin("Render Info", &imGuiState.showRinfo);                          // Create a window called "Hello, world!" and append into it.

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

    ImGui::ColorEdit3("Clear color", (float*)&imGuiState.clearColor); // Edit 3 floats representing a color

    ImGui::Spacing();
    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("Window width %d", windowWidth);
    ImGui::Text("Window height %d", windowHeight);


    ImGui::End();
}


inline void
RoastDisplay::ShowConsole() {

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
    imGuiState.showRinfo = !imGuiState.showRinfo;
}
