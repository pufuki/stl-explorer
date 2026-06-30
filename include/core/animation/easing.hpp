#pragma once

namespace stl_explorer {
namespace animation {

double linear(double t);
double easeIn(double t);
double easeOut(double t);
double easeInOut(double t);
double bounce(double t);
double elastic(double t);

double applyEasing(double t, int type);

} // namespace animation
} // namespace stl_explorer
