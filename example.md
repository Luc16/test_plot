
# Features

## Core Features
- Real-time data updates
- Multiple plot types (line, scatter, bar, histogram)

## Interactivity
- Movable/resizable plot elements
- Export to various formats

## Customization
- Grid lines and ticks
- Axis labels and titles
- Font styles and sizes
- Plot themes
- Custom markers and line styles

# Code examples

## Static Plot


```C++

#include "iglp.hpp"
#include <vector>
  
int main() {
    std::vector<float> x = ...
    std::vector<float> y = ...

    iglp::addPlot(iglp::Plot(x, y))
    
    iglp::show()

    return 0;
}

```

## Interactive Plot

```C++

#include "iglp.hpp"
#include <vector>
  
int main() {
    std::vector<float> x = ...
    std::vector<float> y = sin(x)

    float scl = 1.0f;

    auto update = [&scl](iglp::Plot& plot) {
        std::transform(plot.xData.begin(), plot.xData.end(), plot.yData.begin(), [scl](auto x) { return scl*sin(x) });
    } 

    
    iglp::addSlider("test", scl, 0.0, 10.0);

    iglp::addPlotter(iglp::Plotter(iglp::Plot(x, y), update))
    
    iglp::show()

    return 0;
}
```


```C++

#include "iglp.hpp"
#include <vector>
  
int main() {
    std::vector<float> x = ...
    std::vector<float> y = sin(x)
    auto testPlot = iglp::Plot(x, y);

    auto color = testPlot.getColor();
    float scl = 1.0f;

    auto update = [&scl, &color](iglp::Plot& plot) {
        std::transform(plot.xData.begin(), plot.xData.end(), plot.yData.begin(), [scl](auto x) { return scl*sin(x) });
        plot.setColor(color);
    } 

    
    iglp::addSlider("test", scl, 0.0, 10.0);
    iglp::addColorPicker("Plot color", color);

    iglp::addPlotter(iglp::Plotter(testPlot, update))
    
    iglp::show()

    return 0;
}
```


```C++

#include "iglp.hpp"
#include <vector>

class SinPlotter: public iglp::Plotter {
public:
    SinPlot(const std::vector<float>& x, const std::vector<float>& y) {
        plot = iglp::Plot(x, y);
        color = plot.getColor();

        iglp::addSlider("scale", scale, 0.0, 10.0);
        iglp::addColorPicker("Plot color", color);
    }

    void updatePlot() override {
        std::transform(plot.xData.begin(), plot.xData.end(), plot.yData.begin(), [scl](auto x) { return scl*sin(x) });
        plot.setColor(color);
    }

private:
    iglp::Plot plot;
    float scale = 1.0f;
    iglp::Color color; 
}


int main() {
    std::vector<float> x = ...
    std::vector<float> y = sin(x)
    auto sinPlotter = SinPlotter(x, y);
    
    iglp::addPlotter(sinPlotter);
    
    iglp::show()

    return 0;
}
```

