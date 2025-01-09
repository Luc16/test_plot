#include "iglp.h"
#include <numbers>

int main() {
  // draw a heart
  std::vector<double> t = iglp::linspace(0.0, 2.0*std::numbers::pi, 1000);
  std::vector<double> x, y;
  std::transform(t.begin(), t.end(), std::back_inserter(x),
                 [](auto t) { return std::pow(16.0*std::sin(t), 3); });
  std::transform(t.begin(), t.end(), std::back_inserter(y),
                 [](auto t) { return 13 * std::cos(t) - 5 * std::cos(2 * t) - 2 * std::cos(3 * t) - std::cos(4 * t); });

  iglp::plot(x, y);
  iglp::show();
  return 0;
}