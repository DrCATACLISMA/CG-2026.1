#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

class material
{
public:
    color Ka;         // Constante Ambiente
    color Kd;         // Constante Difusa
    color Ks;         // Constante Especular
    color Kr;         // Constante de Reflexão (Mirror/Espelho)
    color Kt;         // Constante de Transparência (Refração)
    double shininess; // Concentração do brilho
    double ior;       // Índice de Refração (ex: 1.0 para ar, 1.5 para vidro)

    material(const color &a, const color &d, const color &s, const color &r, const color &t, double n, double i)
        : Ka(a), Kd(d), Ks(s), Kr(r), Kt(t), shininess(n), ior(i) {}
};

#endif