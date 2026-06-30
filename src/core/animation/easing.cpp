#include "core/animation/easing.hpp"
#include <cmath>
#include <algorithm>

namespace stl_explorer {
namespace animation {

double linear(double t) {
    return t;
}

double easeIn(double t) {
    return t * t * t;
}

double easeOut(double t) {
    return 1.0 - std::pow(1.0 - t, 3.0);
}

double easeInOut(double t) {
    return t < 0.5
        ? 4.0 * t * t * t
        : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
}

double bounce(double t) {
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (t < 1.0 / d1) {
        return n1 * t * t;
    } else if (t < 2.0 / d1) {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    } else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    } else {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}

double elastic(double t) {
    const double c4 = (2.0 * M_PI) / 3.0;

    return t == 0.0 ? 0.0
        : t == 1.0 ? 1.0
        : std::pow(2.0, -10.0 * t) * std::sin((t * 10.0 - 0.75) * c4) + 1.0;
}

double applyEasing(double t, int type) {
    switch (type) {
        case 0:  return linear(t);
        case 1:  return easeIn(t);
        case 2:  return easeOut(t);
        case 3:  return easeInOut(t);
        case 4:  return bounce(t);
        case 5:  return elastic(t);
        default: return t;
    }
}

} // namespace animation
} // namespace stl_explorer
