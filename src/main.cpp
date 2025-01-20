#include "iglp.hpp"
#include <numbers>
#include <ranges>

int main() {
  // draw a heart
  std::vector<double> t = iglp::linspace(0.0, 2.0*std::numbers::pi, 50);
  std::vector<double> x, y;
  std::transform(t.begin(), t.end(), std::back_inserter(x),
                 [](auto t) { return std::pow(16.0*std::sin(t), 3); });
  std::transform(t.begin(), t.end(), std::back_inserter(y),
                 [](auto t) { return 13 * std::cos(t) - 5 * std::cos(2 * t) - 2 * std::cos(3 * t) - std::cos(4 * t); });



  iglp::addSlider("test", y[4], 0.0, 10.0);
  iglp::plot(iglp::Plot("heart", x, y));


  auto x2 = iglp::linspace(-4000, 4000, 100);
  auto y2 = std::vector<int>(x2.size());
  std::transform(x2.begin(), x2.end(), y2.begin(), [](auto x) {
      return (30*x)/8000;
  });

  iglp::plot(iglp::Plot("line", x2, y2));

  iglp::addDrag("test line", y2[50], 0.1, -10, 10);


  iglp::show();

//  auto x = iglp::linspace(0.0, std::numbers::pi, 100);
//  auto y = std::vector<double>(x.size());
//
//  double tau = 2 * std::numbers::pi;
//  double beta = 0.5;
//
//  std::transform(x.begin(), x.end(), y.begin(), [tau, beta](auto x) {
//    return std::sin(tau * x) * std::pow(x, beta);
//  });
//
//
//
//  iglp::plot(x, y);


  return 0;
}