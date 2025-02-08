//
// Created by luc on 08/01/25.
//

#ifndef TEST_PLOT_PLOT_H
#define TEST_PLOT_PLOT_H

#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <variant>

#include "plot.hpp"
#include "screen_saver.hpp"
#include "control_panel.hpp"
#include "plotter.hpp"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace iglp {

    class Figure {
    public:
        Figure() = default;
        ~Figure() = default;

        void show();


      template<typename T>
      void addSlider(const std::string& name, T& value, T min, T max) { m_controlPanel.addSlider(name, value, min, max); }

      template<typename T>
      void addDrag(const std::string &name, T &value, float speed, T min, T max) { m_controlPanel.addDrag(name, value, speed, min, max); }

      void addCheckBox(const std::string& name, bool& value) { m_controlPanel.addCheckBox(name, value); }

      template<typename T>
      void addPlot(Plot<T>& plot) {
        m_plots.push_back(std::make_unique<Plot<T>>(std::move(plot)));
      }

      template<typename T>
      void addPlotter(Plotter<T>& plotter) {
        m_plotters.push_back(std::make_shared<Plotter<T>>(std::move(plotter)));
      }

      template<typename T>
      void addPlotter(const std::shared_ptr<BasePlotter<T>>& plotter) {
        m_plotters.push_back(plotter);
      }

      void addPlotter(const std::shared_ptr<IPlotter>& plotter) {
        m_plotters.push_back(plotter);
      }

    private:

        static void HelpMarker(const char* desc);

    private:
        ScreenSaver m_screenSaver;
        ControlPanel m_controlPanel;

        std::vector<std::shared_ptr<IPlotter>> m_plotters;
        std::vector<std::unique_ptr<BasePlot>> m_plots;


    };


void Figure::show() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) exit(1);

  // GL 4.0 + GLSL 450
  const char *glsl_version = "#version 450";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create window with graphics context
  const int width = 1280;
  const int height = 720;
  GLFWwindow *window = glfwCreateWindow(width, height, "Plot window", nullptr, nullptr);
  if (window == nullptr) exit(1);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    exit(1);
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;
//        io.IniFilename = "imgui2.ini";


  // Setup Dear ImGui style
  //    ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
//        bool recording = false;
  bool save_graph = false;
  bool is_first_frame = true;

  ImGuiWindowClass window_class;
  window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;


  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto& plotter : m_plotters) {
      plotter->updatePlot();
    }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
      if (m_controlPanel.hasWidgets()) {
        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);
        ImGui::DockBuilderDockWindow("Control Panel", dock_id_left);
      }

      ImGui::DockBuilderDockWindow("Image Window", dockspace_id);
      ImGui::DockBuilderFinish(dockspace_id);
    }

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Save", nullptr, false, true)) {
          save_graph = true;
        }
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

    if (m_controlPanel.hasWidgets()) {
      m_controlPanel.draw();
    }

    // Image Window
    {
      ImGui::SetNextWindowClass(&window_class);
//            ImGui::Begin("Image Window");
      ImGui::Begin("Image Window", nullptr, ImGuiWindowFlags_NoTitleBar);


//            ImGui::ColorEdit3("plot color", (float*)&plot_color); // Edit 3 floats representing a color

      if (ImPlot::BeginPlot(" a", ImGui::GetContentRegionAvail())) {
        for (const auto &plot : m_plots) {
          plot->draw();
        }
        for (const auto &plotter : m_plotters) {
          plotter->draw();
        }


        ImPlot::EndPlot();
      }

      if (save_graph) {
        auto start = std::chrono::high_resolution_clock::now();
        save_graph = false;
        m_screenSaver.saveCurrentWindowToPNG("graph.png");
        std::cout << "Time to save graph: " << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
      }

      ImGui::End();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
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
}

void Figure::HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}


} // namespace iglp

#endif //TEST_PLOT_PLOT_H
