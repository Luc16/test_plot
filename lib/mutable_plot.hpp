//
// Created by luc on 06/02/25.
//

#ifndef TEST_PLOT_LIB_MUTABLE_PLOT_HPP_
#define TEST_PLOT_LIB_MUTABLE_PLOT_HPP_

#include <utility>
#include <vector>
#include <imgui.h>
#include <memory>
#include "implot.h"
#include <iostream>

namespace iglp {

template<typename T>
class BasePlotter;

class BaseMutablePlot {
 public:
  virtual ~BaseMutablePlot() = default;

 private:
  virtual void draw() const = 0;

};

template<typename T>
class MutablePlot: public BaseMutablePlot {
 public:
    MutablePlot(std::string name, const std::vector<T>& x, const std::vector<T>& y)
        : xData(x), yData(y), m_name(std::move(name)) {
        assert(xData.size() == yData.size() && "Size mismatch between x and y data");
    }

    MutablePlot(const MutablePlot& other) = delete;

    MutablePlot(MutablePlot&& other) noexcept
        : xData(std::move(other.xData)),
            yData(std::move(other.yData)),
            m_name(std::move(other.m_name)),
            m_plotColor(other.m_plotColor) {}

    ~MutablePlot() override = default;

    [[nodiscard]] const std::string& getName() const { return m_name; }

 private:
  void draw() const override;
    friend class BasePlotter<T>;
 public:
    std::vector<T> xData;
    std::vector<T> yData;
 private:
    std::string m_name;
    ImVec4 m_plotColor = ImVec4(0.298039, 0.447059, 0.690196, 1);
};

template<typename T>
void MutablePlot<T>::draw() const {
  assert(xData.size() == yData.size() && "Size mismatch between x and y data");
//      ImPlot::SetNextLineStyle(m_plotColor);
//      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
  ImPlot::PlotLine(m_name.c_str(), xData.data(), yData.data(), xData.size());
}


}

#endif //TEST_PLOT_LIB_MUTABLE_PLOT_HPP_
