//
// Created by luc on 08/01/25.
//

#ifndef TEST_PLOT_IGLP_H
#define TEST_PLOT_IGLP_H

#include "figure.hpp"
#include <memory>
#include <any>
#include <variant>

namespace iglp {
  static Figure globalFigure;

  template<typename T>
  std::vector<T> linspace(T start, T end, uint32_t num) {
    std::vector<T> vec;
    if (num == 0) {
      return vec;
    } else if (num == 1) {
      vec.push_back(start);
      return vec;
    }
    T delta = (end - start) / (num - 1);
    for (uint32_t i = 0; i < num - 1; ++i) {
      vec.push_back(start + delta * i);
    }
    vec.push_back(end);
    return vec;
  }

  template<typename T>
  void plot(Plot<T> plot) {
    globalFigure.addPlot(plot);
  }

  template<typename T>
  void plot(Plotter<T> plotter) {
    globalFigure.addPlotter(plotter);
  }

  template<typename T>
  void plot(const std::shared_ptr<BasePlotter<T>>& plotter) {
    globalFigure.addPlotter(plotter);
  }

  void plot(const std::shared_ptr<IPlotter>& plotter) {
    globalFigure.addPlotter(plotter);
  }

  template<typename T>
  void addSlider(const std::string& name, T& value, T min, T max) {
    globalFigure.addSlider(name, value, min, max);
  }

  template<typename T>
  void addDrag(const std::string& name, T& value, float speed, T min, T max) {
    globalFigure.addDrag(name, value, speed, min, max);
  }

  void addCheckBox(const std::string& name, bool& value) {
    globalFigure.addCheckBox(name, value);
  }

  void show() {
    globalFigure.show();
  }

}

#endif //TEST_PLOT_IGLP_H
