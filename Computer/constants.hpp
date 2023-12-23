
#include "vector.hpp"

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

using Vector3f = vector::Vector<3, float>;
using Vector2f = vector::Vector<2, float>;

static float square(float x) {
    return x*x;
}

#endif
