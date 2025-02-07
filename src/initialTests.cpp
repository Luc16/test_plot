#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include <cstdio>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <cassert>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

FILE* ffmpegFile = nullptr;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void saveFrameBufferToPNG(GLuint fbo, int width, int height, const char* filename) {
    // Allocate memory to store the pixel data
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<unsigned char> pixels(width * height * 4);

    // Bind the framebuffer and read the pixel data
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Time to read pixels: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    stbi_flip_vertically_on_write(true);
    // Save the pixel data to a PNG file
    stbi_write_png(filename, width, height, 4, pixels.data(), width * 4);

    std::cout << "Time to write PNG: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
}

void saveCurrentWindowToPNG(const char* filename) {
    static bool created_frame_buffer = false;
    static GLuint fbo;
    static GLuint texture;
    static GLuint rbo;

    int width  = (int)ImGui::GetWindowSize().x;
    int height = (int)ImGui::GetWindowSize().y;

    if (!created_frame_buffer) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        created_frame_buffer = true;
    }

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    auto color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    glClearColor(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    auto dl = ImGui::GetWindowDrawList();

    static ImDrawData drawData = ImDrawData();
    drawData.Clear();
    drawData.Valid = true;
    drawData.DisplayPos = ImGui::GetWindowPos();
    drawData.DisplaySize = ImGui::GetWindowSize();
    drawData.FramebufferScale = ImVec2(1.0f, 1.0f);
    drawData.AddDrawList(dl);


    ImGui_ImplOpenGL3_RenderDrawData(&drawData);


    saveFrameBufferToPNG(fbo, width, height, filename);

}

void startRecording(const char* filename, int width, int height) {
    // Allocate memory to store the pixel data
    width += width % 2;
    height += height % 2;

    std::stringstream ss;
    ss << "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s " << width << "x" << height << " -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " << filename;
    auto cmd = ss.str();

// open pipe to ffmpeg's stdin in binary write mode
    assert(ffmpegFile == nullptr);
    ffmpegFile = popen(cmd.c_str(), "w");
    if (ffmpegFile == nullptr) {
        std::cerr << "Error opening ffmpeg pipe" << std::endl;
        exit(1);
    }
}

void endRecording() {
    assert(ffmpegFile != nullptr);
    pclose(ffmpegFile);
    ffmpegFile = nullptr;
}

void recordFrame(GLuint fbo, int width, int height) {
    // Allocate memory to store the pixel data
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<unsigned char> pixels(width * height * 4);

    // Bind the framebuffer and read the pixel data
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Writing frame to: " << ffmpegFile  << std::endl;

    // Write the pixel data to the ffmpeg pipe
    fwrite(pixels.data(), sizeof(unsigned char)*pixels.size(), 1, ffmpegFile);
    std::cout << "Time to read pixels: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
}


// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    const int width = 1280;
    const int height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    io.IniFilename = "imgui2.ini";


    // Setup Dear ImGui style
//    ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool recording = false;
    bool show_demo_window = false;
    bool show_another_window = false;
    bool save_graph = false;
    ImVec4 plot_color = ImVec4(0.298039, 0.447059, 0.690196, 1);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

    auto prev_size = ImVec2(0, 0);

    bool is_first_frame = true;
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

//         1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool open = true;
        ImGui::Begin("DockSpace Demo", &open, window_flags);

        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);


        auto dockspace_id = ImGui::GetID("WindowDockSpace");

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        if (is_first_frame) {
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);

//            ImGui::DockBuilderDockWindow("ImPlot Demo", dockspace_id);
            ImGui::DockBuilderDockWindow("Image Window", dockspace_id);
            ImGui::DockBuilderDockWindow("Settings Window", dock_id_left);
            ImGui::DockBuilderFinish(dockspace_id);
        }




        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {

                ImGui::Separator();

                ImGui::Separator();

                if (ImGui::MenuItem("Close", nullptr, false, true))
                    open = false;
                ImGui::EndMenu();
            }
            HelpMarker(
                    "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
                    "- Drag from window title bar or their tab to dock/undock." "\n"
                    "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
                    "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
                    "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
                    "This demo app has nothing to do with enabling docking!" "\n\n"
                    "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
                    "Read comments in ShowExampleAppDockSpace() for more details.");

            ImGui::EndMenuBar();
        }

//         Settings window
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowClass(&window_class);
            ImGui::Begin("Settings Window", nullptr, ImGuiWindowFlags_NoTitleBar); // Create a window called "Settings Window" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::Text("Image window size: %f, %f", prev_size.x, prev_size.y);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
            ImGui::Separator();
            ImGui::ColorEdit3("plot color", (float*)&plot_color); // Edit 3 floats representing a color
            ImGui::Text("Triangle Colors");
//            ImGui::ColorEdit3("Vertex 1", (float*)&vertices[3]);
//            ImGui::ColorEdit3("Vertex 2", (float*)&vertices[10]);
//            ImGui::ColorEdit3("Vertex 3", (float*)&vertices[17]);

            ImGui::Separator();

            if (ImGui::Button("save")) {
                save_graph = true;
//                saveFrameBufferToPNG(fbo, (int) prev_size.x, (int) prev_size.y, "frame.png");
            }

            if (ImGui::Button("Start recording")) {
                if (!recording) {
                    startRecording("output.mp4", (int) prev_size.x, (int) prev_size.y);
                    recording = true;
                    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);
                }
            }

            if (ImGui::Button("End recording")) {
                if (recording) {
                    endRecording();
                    recording = false;
                    glfwSetWindowAttrib(window, GLFW_RESIZABLE, true);
                }
            }

            if (recording) {
                ImGui::Text("Recording...");
            }

            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            if (ImGui::Button("test_save_window")) {
                saveCurrentWindowToPNG("window.png");
            }
            ImGui::End();
        }

        // Image Window
        {
            ImGui::SetNextWindowClass(&window_class);
//            ImGui::Begin("Image Window");
            ImGui::Begin("Image Window", nullptr, ImGuiWindowFlags_NoTitleBar);


            srand(0);
            static float xs1[100], ys1[100];
            for (int i = 0; i < 100; ++i) {
                xs1[i] = i * 0.01f;
                ys1[i] = xs1[i] + 0.1f * ((float) rand() / (float) RAND_MAX);
            }
            static float xs2[50], ys2[50];
            for (int i = 0; i < 50; i++) {
                xs2[i] = 0.25f + 0.2f * ((float) rand() / (float) RAND_MAX);
                ys2[i] = 0.75f + 0.2f * ((float) rand() / (float) RAND_MAX);
            }




            if (ImPlot::BeginPlot("Scatter Plot", ImGui::GetContentRegionAvail())) {
//                ImPlot::SetupAxes("Arbitrary","Random",ImPlotAxisFlags_NoGridLines,ImPlotAxisFlags_NoGridLines);
                ImPlot::SetNextLineStyle(plot_color);
//                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 5, plot_color, 1, ImVec4(0, 0, 0, 1));
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                ImPlot::PlotLine("Data 1", xs1, ys1, 100);
                ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImPlot::GetColormapColor(1), IMPLOT_AUTO,
                                           ImPlot::GetColormapColor(1));
                ImPlot::PlotScatter("Data 2", xs2, ys2, 50);
                ImPlot::PopStyleVar();
                ImPlot::EndPlot();
            }
            ImGui::ColorEdit3("plot color", (float*)&plot_color); // Edit 3 floats representing a color

            if (save_graph) {
                auto start = std::chrono::high_resolution_clock::now();
                save_graph = false;
                saveCurrentWindowToPNG("graph.png");
                std::cout << "Time to save graph: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
            }


            ImGui::End();
        }


        ImGui::End();

        ImPlot::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);

        is_first_frame = false;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}