#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>

// CONTANTES

// C++ Std

using std::make_shared;
using std::shared_ptr;

// Constantes

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Funções Uteis

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double random_double()
{
    // Retorna um número real aleatório entre 0.0 e 1.0
    return std::rand() / (RAND_MAX + 1.0);
}

// Bibliotecas

#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vector.hpp"

#endif