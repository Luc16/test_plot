//
// Created by luc on 08/01/25.
//

#ifndef TEST_PLOT_IGLP_H
#define TEST_PLOT_IGLP_H

#include "plot.h"
#include <memory>
#include <any>
#include <variant>

namespace iglp {
  static bool globalInit = false;
  static std::variant<
      Plot<signed char>,
      Plot<unsigned char>,
      Plot<signed short>,
      Plot<unsigned short>,
      Plot<signed int>,
      Plot<unsigned int>,
//      Plot<signed long>,
//      Plot<unsigned long>,
      Plot<signed long long>,
      Plot<unsigned long long>,
      Plot<float>,
      Plot<double>
//      Plot<long double>
  > globalPlot;

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
  void plot(std::vector<T>& xData, std::vector<T>& yData) {
    if (!globalInit) {
      globalInit = true;
      globalPlot = Plot<T>();
    }
    std::get<Plot<T>>(globalPlot).plot(xData, yData);
  }

  void show() {
    std::visit([](auto& plot) { plot.show(); }, globalPlot);
  }

}

#endif //TEST_PLOT_IGLP_H
