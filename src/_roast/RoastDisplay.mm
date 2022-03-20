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


                RDGui::ShowHelloWorld(&imGuiState);

                ImVec4 clear_color = ImVec4(imGuiState.clearColor[0], imGuiState.clearColor[1], imGuiState.clearColor[2], imGuiState.clearColor[3]);

                // 3. Show another simple window.
                if (imGuiState.showAnotherWindow)
                {
                    RDGui::ShowAnotherWindow(&imGuiState);
                }

                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(pGlfwWindow, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
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

            return finish();
            break;
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
                    int width, height;
                    glfwGetFramebufferSize(pGlfwWindow, &width, &height);
                    mtlRenderer.layer.drawableSize = CGSizeMake(width, height);
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

                    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
                    RDGui::ShowHelloWorld(&imGuiState);

                    // 3. Show another simple window.
                    if (imGuiState.showAnotherWindow)
                    {
                        RDGui::ShowAnotherWindow(&imGuiState);
                    }

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

            return finish();
    }

    return 0;
}



int
RoastDisplay::finish() {
    return 0;
}

