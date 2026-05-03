#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.hpp"

/*
    No contexto do livro, essa classe é usada para controlar o intervalo válido de t ao longo de um raio — ou seja,
    até onde o raio pode "enxergar"

*/

class interval
{
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // o intervalo padrão é vazio

    interval(double min, double max) : min(min), max(max) {} // todos os numéros reais

    // retorna o tamanho do intervalo
    double size() const
    {
        return max - min;
    }

    // verifica se x está dentro do intervalo
    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    // verifia se x está estritamente dentro
    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif