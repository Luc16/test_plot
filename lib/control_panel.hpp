//
// Created by luc on 16/01/25.
//

#ifndef TEST_PLOT_LIB_CONTROL_PANEL_HPP_
#define TEST_PLOT_LIB_CONTROL_PANEL_HPP_

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include "imgui.h"
#include "imgui_internal.h"

namespace iglp {

class ControlPanel {

 public:
  ControlPanel();
  ~ControlPanel() = default;

  void draw();

  template<typename T>
  void addSlider(const std::string& name, T& value, T min, T max);

  [[nodiscard]] bool hasWidgets() const { return !m_widgets.empty(); }
  size_t getNumWidgets() const { return m_widgets.size(); }

 private:

  template<typename T>
  static constexpr ImGuiDataType_ getType();
  ImGuiWindowClass m_window_class{};
  std::vector<std::function<void()>> m_widgets;

};

template<typename T>
constexpr ImGuiDataType_ ControlPanel::getType() {
  if (std::is_same_v<T, signed char>) {
    return ImGuiDataType_S8;
  } else if (std::is_same_v<T, unsigned char>) {
    return ImGuiDataType_U8;
  } else if (std::is_same_v<T, short>) {
    return ImGuiDataType_S16;
  } else if (std::is_same_v<T, unsigned short>) {
    return ImGuiDataType_U16;
  } else if (std::is_same_v<T, int> || std::is_same_v<T, int32_t>) {
    return ImGuiDataType_S32;
  } else if (std::is_same_v<T, unsigned int> || std::is_same_v<T, uint32_t>) {
    return ImGuiDataType_U32;
  } else if (std::is_same_v<T, long long> || std::is_same_v<T, int64_t>) {
    return ImGuiDataType_S64;
  } else if (std::is_same_v<T, unsigned long long> || std::is_same_v<T, uint64_t>) {
    return ImGuiDataType_U64;
  } else if (std::is_same_v<T, float>) {
    return ImGuiDataType_Float;
  } else if (std::is_same_v<T, double>) {
    return ImGuiDataType_Double;
  }
  throw std::runtime_error("Unsupported type");
}

template<typename T>
void ControlPanel::addSlider(const std::string &name, T &value, T min, T max) {
  ImGuiDataType_ type = getType<T>();
  m_widgets.push_back([name, &value, min, max, type]() {
    ImGui::SliderScalar(name.c_str(), type, &value, &min, &max);
  });
}

}


#endif //TEST_PLOT_LIB_CONTROL_PANEL_HPP_
