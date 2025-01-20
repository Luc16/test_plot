//
// Created by luc on 16/01/25.
//

#include "control_panel.hpp"
namespace iglp {

ControlPanel::ControlPanel() {
  m_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
}

void ControlPanel::draw() {

    ImGui::SetNextWindowClass(&m_window_class);
    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoTitleBar);

    for (const auto& widget : m_widgets) {
      widget();
    }

    ImGui::End();
  }


void ControlPanel::addCheckBox(const std::string &name, bool &value) {
  m_widgets.emplace_back([name, &value]() {
    ImGui::Checkbox(name.c_str(), &value);
  });

}
}
