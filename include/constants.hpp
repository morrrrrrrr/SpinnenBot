
#include "vector.hpp"

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define M_PI 3.14159265358979323846 /* pi */

using Vector3f = vector::Vector<3, float>;
using Vector2f = vector::Vector<2, float>;

static float square(float x) {
    return x*x;
}

#endif
