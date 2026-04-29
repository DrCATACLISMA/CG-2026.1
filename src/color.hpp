#ifndef COLOR_H
#define COLOR_H

#include "vector.hpp"

#include <iostream>

// alias
using color = vec3;

void write_color(std::ostream& out, const color& pixel_color) {

    // uso dos getters x, y e z do vector_func, retorno das componentes
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    // impressão da cores RGB
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif