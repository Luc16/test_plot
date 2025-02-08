//
// Created by luc on 18/01/25.
//

#ifndef TEST_PLOT_LIB_PLOT_HPP_
#define TEST_PLOT_LIB_PLOT_HPP_

#include <utility>
#include <vector>
#include <imgui.h>
#include <memory>
#include "implot.h"
#include <iostream>

namespace iglp {

  class BasePlot {
   public:
    virtual ~BasePlot() = 0;

   private:

    virtual void draw() const = 0;

    friend class Figure;
  };

  inline BasePlot::~BasePlot() = default;

  template<typename T>
  class Plot: public BasePlot {
  public:
    Plot(std::string name, const std::vector<T>& x, const std::vector<T>& y) : xData(x), yData(y), m_name(std::move(name)) {
      assert(xData.size() == yData.size() && "Size mismatch between x and y data");
      // std::cout << "Plot " << m_name << " created" << std::endl;
    } 

    Plot(const Plot& other) = delete;

    Plot(Plot&& other) noexcept
      : xData(std::move(other.xData)),
        yData(std::move(other.yData)),
        m_name(std::move(other.m_name)),
        m_plotColor(other.m_plotColor) {
      // m_name += "_moved";
      // std::cout << "Plot " << m_name << " moved" << std::endl;
    }

    // Plot& operator=(Plot&& other) noexcept {
    //   if (this != &other) {
    //     xData = std::move(other.xData);
    //     yData = std::move(other.yData);
    //     m_name = std::move(other.m_name);
    //     m_plotColor = other.m_plotColor;
    //   }
    //   return *this;
    // }

    ~Plot() override {
      // std::cout << "Plot " << m_name << " destroyed" << std::endl;
    }

    [[nodiscard]] const std::string& getName() const { return m_name; }
    [[nodiscard]] Plot<T>&& move() { return std::move(*this); }

   private:
    void draw() const override;

    std::vector<T> xData;
    std::vector<T> yData;
    std::string m_name;
    ImVec4 m_plotColor = ImVec4(0.298039, 0.447059, 0.690196, 1);
  };


    template<typename T>
    void Plot<T>::draw() const {
      assert(xData.size() == yData.size() && "Size mismatch between x and y data");
//      ImPlot::SetNextLineStyle(m_plotColor);
//      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine(m_name.c_str(), xData.data(), yData.data(), xData.size());
    }


}

#endif //TEST_PLOT_LIB_PLOT_HPP_
