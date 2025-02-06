//
// Created by luc on 06/02/25.
//

#ifndef TEST_PLOT_LIB_PLOTTER_HPP_
#define TEST_PLOT_LIB_PLOTTER_HPP_

#include <functional>
#include "plot.hpp"
#include "mutable_plot.hpp"
namespace iglp {

class IPlotter {
 public:
  virtual void updatePlot() = 0;

  virtual void draw() const = 0;

  virtual ~IPlotter() = default;
};

template<typename T>
class BasePlotter: public IPlotter {
 public:
  explicit BasePlotter(MutablePlot<T> plot): m_plot(std::move(plot)) {}

  virtual void update(iglp::MutablePlot<T> &plot) = 0;

  void updatePlot() override {
    update(m_plot);
  }

  void draw() const override {
      m_plot.draw();
  }

  ~BasePlotter() override = default;

 protected:
  iglp::MutablePlot<T> m_plot;
};

template<typename T>
class Plotter: public BasePlotter<T> {
 public:
  explicit Plotter(iglp::MutablePlot<T> plot, const std::function<void(iglp::MutablePlot<T>&)>& func): BasePlotter<T>(std::move(plot)), m_func(func) {}

  Plotter(const Plotter&) = delete;
  Plotter& operator=(const Plotter&) = delete;

  Plotter(Plotter&& other) noexcept : BasePlotter<T>(std::move(other.m_plot)), m_func(std::move(other.m_func)) {}

  [[nodiscard]] Plotter<T>&& move() {
    return std::move(*this);
  }
  void update(iglp::MutablePlot<T> &plot) override {
    m_func(plot);
  }

private:
  std::function<void(iglp::MutablePlot<T>&)> m_func;
};


} // namespace iglp
#endif //TEST_PLOT_LIB_PLOTTER_HPP_
