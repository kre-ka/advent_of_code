#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "SafeInt/SafeInt.hpp"

template <typename T>
struct Vector2i {
#ifdef DEBUG
  SafeInt<T> x;
  SafeInt<T> y;
#endif
#ifndef DEBUG
  T x;
  T y;
#endif

  Vector2i& operator+=(const Vector2i& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  friend Vector2i operator+(Vector2i lhs, const Vector2i& rhs) {
    lhs += rhs;
    return lhs;
  }

  Vector2i operator-() const { return {-x, -y}; }

  Vector2i& operator-=(const Vector2i& rhs) {
    *this += -rhs;
    return *this;
  }

  friend Vector2i operator-(Vector2i lhs, const Vector2i& rhs) {
    lhs -= rhs;
    return lhs;
  }

  Vector2i& operator*=(T rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
  }

  friend Vector2i operator*(Vector2i lhs, T rhs) {
    lhs *= rhs;
    return lhs;
  }

  /*
  Can be used only with horizontal and vertical vectors
  */
  Vector2i normalize() {
#ifdef DEBUG
    if (!(x == 0 || y == 0)) {
      throw std::runtime_error(
          "Vector2i.normalize() can be used only with horizontal and vertical "
          "vectors");
    }
#endif
    // you can do this if one term is 0
    T norm = abs(x + y);
    return {x / norm, y / norm};
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector2i<T>& obj) {
  os << "[" << (T)obj.x << ", " << (T)obj.y << "]";
  return os;
}