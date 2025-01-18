//
// Created by luc on 18/01/25.
//

#ifndef TEST_PLOT_LIB_PLOT_HPP_
#define TEST_PLOT_LIB_PLOT_HPP_

#include <vector>
#include <imgui.h>
#include <memory>
#include "implot.h"
#include <iostream>

namespace iglp {

  class BasePlot {
   public:
    virtual ~BasePlot() = 0;

    virtual void draw() const = 0;
  };

  inline BasePlot::~BasePlot() = default;

  template<typename T>
  class Plot: public BasePlot {
  public:
    Plot(std::vector<T>& xData, std::vector<T>& yData) : m_XData(xData), m_YData(yData) {
      assert(m_XData.size() == m_YData.size() && "Size mismatch between x and y data");
    }

    ~Plot() override = default;

    void draw() const override;

   private:
    std::vector<T>& m_XData;
    std::vector<T>& m_YData;
    ImVec4 m_plotColor = ImVec4(0.298039, 0.447059, 0.690196, 1);
  };


    template<typename T>
    void Plot<T>::draw() const {
      assert(m_XData.size() == m_YData.size() && "Size mismatch between x and y data");
      ImPlot::SetNextLineStyle(m_plotColor);
//      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("Data 1", m_XData.data(), m_YData.data(), m_XData.size());
    }


}

#endif //TEST_PLOT_LIB_PLOT_HPP_
