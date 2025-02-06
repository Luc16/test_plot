#include "iglp.hpp"
#include "plotter.hpp"
#include <numbers>
#include <ranges>

void plotHeart() {
  // draw a heart
  std::vector<double> t = iglp::linspace(0.0, 2.0*std::numbers::pi, 50);
  std::vector<double> x, y;
  std::transform(t.begin(), t.end(), std::back_inserter(x),
                 [](auto t) { return std::pow(16.0*std::sin(t), 3); });
  std::transform(t.begin(), t.end(), std::back_inserter(y),
                 [](auto t) { return 13 * std::cos(t) - 5 * std::cos(2 * t) - 2 * std::cos(3 * t) - std::cos(4 * t); });

  iglp::plot(iglp::Plot("heart", x, y));
}


void staticPlotExample() {
  plotHeart();

  auto x2 = iglp::linspace(-4000, 4000, 100);
  auto y2 = std::vector<int>(x2.size());
  std::transform(x2.begin(), x2.end(), y2.begin(), [](auto x) {
    return (30*x)/8000;
  });

  auto line = iglp::Plot("line", x2, y2);
  iglp::plot(line.move());

  iglp::show();
}

void dynamicPlotExample() {
  std::vector<double> x = iglp::linspace(-2.0 * std::numbers::pi, 2.0 * std::numbers::pi, 1000);
  std::vector<double> y;
  std::transform(x.begin(), x.end(), std::back_inserter(y),
                 [](auto x) { return std::sin(x); });

  double amp = 1, freq = 1;
  iglp::Plotter<double> plotter(iglp::MutablePlot<double>("heart", x, y), [&amp, &freq](iglp::MutablePlot<double> &plot) {
    std::transform(plot.xData.begin(), plot.xData.end(), plot.yData.begin(), [&amp, &freq](auto x) {
      return amp * std::sin(freq * x);
    });
  });

  iglp::addDrag("amplitude", amp, 0.01f, -2.0, 2.0);
  iglp::addDrag("frequency", freq, 0.01f, 0.0, 5.0);

  iglp::plot(plotter.move());
  iglp::show();
}

int main() {
//  staticPlotExample();
    dynamicPlotExample();
  return 0;
}