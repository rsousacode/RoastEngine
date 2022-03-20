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
    VkRender vkRenderer;
    MtlRender mtlRenderer{};
    OglRender oglRender;

    if(RType == RE_OPENGL) {
        oglRender.setupAdapter(window, windowWidth, windowHeight);
        pGlfwWindow = oglRender.GetGlfwWindow();
        setupInput();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(pGlfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 150");


        while(!glfwWindowShouldClose(pGlfwWindow)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            imGuiState.showDemoWindow = true;
            imGuiState.showAnotherWindow = false;
            ImVec4 clear_color = ImVec4(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            if (imGuiState.showDemoWindow)
                ImGui::ShowDemoWindow(&imGuiState.showAnotherWindow);

            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &imGuiState.showDemoWindow);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &imGuiState.showAnotherWindow);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // 3. Show another simple window.
            if (imGuiState.showAnotherWindow)
            {
                ImGui::Begin("Another Window", &imGuiState.showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    imGuiState.showAnotherWindow = false;
                ImGui::End();
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
    }

    if(RType == RE_VULKAN) {
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
    }

    if(RType == RE_VULKAN) {

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
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor[0], clearColor[1],
                                                                                                        clearColor[2], clearColor[3]);;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    mtlRenderer.renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:mtlRenderer.renderPassDescriptor];
                    [renderEncoder pushDebugGroup:@"ImGui demo"];

                    // Start the Dear ImGui frame
                    ImGui_ImplMetal_NewFrame(mtlRenderer.renderPassDescriptor);
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();

                    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
                    if (imGuiState.showDemoWindow)
                        ImGui::ShowDemoWindow(&imGuiState.showDemoWindow);

                    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
                    {
                        static float f = 0.0f;
                        static int counter = 0;

                        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                        ImGui::Checkbox("Demo Window", &imGuiState.showDemoWindow);      // Edit bools storing our window open/close state
                        ImGui::Checkbox("Another Window", &imGuiState.showAnotherWindow);

                        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                        ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

                        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                            counter++;
                        ImGui::SameLine();
                        ImGui::Text("counter = %d", counter);

                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                        ImGui::End();
                    }

                    // 3. Show another simple window.
                    if (imGuiState.showAnotherWindow)
                    {
                        ImGui::Begin("Another Window", &imGuiState.showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                        ImGui::Text("Hello from another window!");
                        if (ImGui::Button("Close Me"))
                            imGuiState.showAnotherWindow = false;
                        ImGui::End();
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

RoastDisplay*
RoastDisplay::Instance() {

    static auto* display = new RoastDisplay;
    return display;
}

