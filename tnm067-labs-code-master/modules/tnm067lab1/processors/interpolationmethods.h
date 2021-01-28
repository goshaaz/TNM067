#pragma once

#include <modules/tnm067lab1/tnm067lab1moduledefine.h>
#include <inviwo/core/util/glm.h>

namespace inviwo {

template <typename T>
struct float_type {
    using type = double;
};

template <>
struct float_type<float> {
    using type = float;
};
template <>
struct float_type<vec3> {
    using type = float;
};
template <>
struct float_type<vec2> {
    using type = float;
};
template <>
struct float_type<vec4> {
    using type = float;
};

namespace TNM067 {
namespace Interpolation {

#define ENABLE_LINEAR_UNITTEST 0
template <typename T, typename F = double>
T linear(const T& a, const T& b, F x) {
    if (x <= 0) return a;
    if (x >= 1) return b;

    return a;
}

// clang-format off
    /*
     2------3
     |      |
    y|  •   |
     |      |
     0------1
        x
    */
    // clang format on
#define ENABLE_BILINEAR_UNITTEST 0
template<typename T, typename F = double> 
T bilinear(const std::array<T, 4> &v, F x, F y) {
    float value = v[0]*(1-x)*(1-y)+v[1]*x*(1-y)+v[2]*(1-x)*y+v[3]*x*y;
    return value;

}


    // clang-format off
    /* 
    a--•----b------c
    0  x    1      2
    */
// clang-format on
#define ENABLE_QUADRATIC_UNITTEST 0
template <typename T, typename F = double>
T quadratic(const T& a, const T& b, const T& c, F x) {
    double value = (1 - x) * (1 - 2*x) * a + 4 * x * (1 - x) * b + x * (2 * x - 1) * c;
    return value;
}

// clang-format off
    /* 
    6-------7-------8
    |       |       |
    |       |       |
    |       |       |
    3-------4-------5
    |       |       |
   y|  •    |       |
    |       |       |
    0-------1-------2
    0  x    1       2
    */
// clang-format on
#define ENABLE_BIQUADRATIC_UNITTEST 0
template <typename T, typename F = double>
T biQuadratic(const std::array<T, 9>& v, F x, F y) {
    double value1 = TNM067::Interpolation::quadratic(v[0], v[1], v[2], x);
    double value2 = TNM067::Interpolation::quadratic(v[3], v[4], v[5], x);
    double value3 = TNM067::Interpolation::quadratic(v[6], v[7], v[8], x);
    double value4 = TNM067::Interpolation::quadratic(value1, value2, value3, y);
    return value4;
}

// clang-format off
    /*
     2---------3
     |'-.      |
     |   -,    |
   y |  •  -,  |
     |       -,|
     0---------1
        x
    */
// clang-format on
#define ENABLE_BARYCENTRIC_UNITTEST 1
template <typename T, typename F = double>
T barycentric(const std::array<T, 4>& v, F x, F y) {
    double value;

    if (y >= -x + 1) {
        x = 1 - x;
        y = 1 - y;
        double A1 = (1 * y) / 2;
        double A2 = (1 * x) / 2;
        double A = 0.5;
        double alpha = A1 / A;
        double beta = A2 / A;
        double gamma = 1 - alpha - beta;
        value = alpha * v[1] + beta * v[2] + gamma * v[3];

    } else {
        double A1 = (1 * x) / 2;
        double A2 = (1 * y) / 2;
        double A = 0.5;
        double alpha = A1 / A;
        double beta = A2 / A;
        double gamma = 1 - alpha - beta;
        value = alpha * v[1] + beta * v[2] + gamma * v[0];
    }

    return value;
}

}  // namespace Interpolation
}  // namespace TNM067
}  // namespace inviwo
