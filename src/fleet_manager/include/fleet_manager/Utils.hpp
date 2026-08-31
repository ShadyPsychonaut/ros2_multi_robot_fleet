#pragma once

#include <cmath>

class Utils
{
public:
  static double norm2D(const double &x, const double &y)
  {
    return std::sqrt(x * x + y * y);
  }
};