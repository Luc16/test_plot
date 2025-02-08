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
  virtual ~IPlotter() = default;

 private:
  virtual void updatePlot() = 0;

  virtual void draw() const = 0;

  friend class Figure;
};

template<typename T>
class BasePlotter: public IPlotter {
 public:
  explicit BasePlotter(MutablePlot<T> plot): plot(std::move(plot)) {}
  explicit BasePlotter(const std::string& name): plot(name, {}, {}) {}

  ~BasePlotter() override = default;

 private:
  virtual void update([[maybe_unused]] iglp::MutablePlot<T> &mutPlot) {};
  virtual void update() {};

  void updatePlot() override {
    update(plot);
    update();
  }

  void draw() const override {
      plot.draw();
  }


 protected:
  iglp::MutablePlot<T> plot;
};

template<typename T>
class Plotter: public BasePlotter<T> {
 public:
  explicit Plotter(iglp::MutablePlot<T> plot, const std::function<void(iglp::MutablePlot<T>&)>& func): BasePlotter<T>(std::move(plot)), m_func(func) {}

  Plotter(const Plotter&) = delete;
  Plotter& operator=(const Plotter&) = delete;

  Plotter(Plotter&& other) noexcept : BasePlotter<T>(std::move(other.plot)), m_func(std::move(other.m_func)) {}

  [[nodiscard]] Plotter<T>&& move() {
    return std::move(*this);
  }

 private:

  void update(iglp::MutablePlot<T> &mutPlot) override {
    m_func(mutPlot);
  }

  std::function<void(iglp::MutablePlot<T>&)> m_func;
};

} // namespace iglp
#endif //TEST_PLOT_LIB_PLOTTER_HPP_
