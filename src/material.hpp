#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

class material
{
public:
    color Ka;         // Constante Ambiente
    color Kd;         // Constante Difusa
    color Ks;         // Constante Especular
    double shininess; // Concentração do brilho

    material(const color &a, const color &d, const color &s, double n)
        : Ka(a), Kd(d), Ks(s), shininess(n) {}
};

#endif