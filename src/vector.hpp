#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.hpp"

// definições e funções auxiliares do vec3

class vec3
{
public:
    double e[3];

    // construtores
    // inicialização com 0
    vec3() : e{0, 0, 0} {}

    // inicialização com double e0 etc...
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    // funções getters, componetes do vetor
    // CONST obrigatoriamente faz a função NÃO modificar o objeto
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // SOBRECARGA DE OPERADOR
    // OPERADOR NEGATIVO, RETORNA AS COMPONENTES DO VETOR NEGATIVOS
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    // OPERADOR DE LEITURA
    /*
        vec3 v(10,20,30);
        double x = v[0];  // 10

        JAVA:

        double get(int i) {
            return e[i];
        }
    */
    double operator[](int i) const { return e[i]; }
    /*
        & significa retorne uma referência ao elemento

        v[0] = 100;

        v.operator[](0) → referência para e[0]
    */
    double &operator[](int i) { return e[i]; }

    /*
        C++ diferencia

        | Situação              | Função usada   |
        | --------------------- | -------------- |
        | leitura (`x = v[0]`)  | versão `const` |
        | escrita (`v[0] = 10`) | versão com `&` |

    */

    // RETORNA O OBJETO, POR REFERENCIA
    vec3 &operator+=(const vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3 &operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3 &operator/=(double t)
    {
        return *this *= 1 / t;
    }

    // CALCULO DO MODULO DO VETOR
    double length() const
    {
        return std::sqrt(length_squared());
    }

    // COMPRIMENTO DO VETOR
    double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;

// Vector Utility Functions

// Impressão
// : pode substituir direto no código, otimização
std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// SOMA DE VETORES
vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// SUBTRAÇÃO DE VETORES
vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// MULTIPLICAÇÃO DE UM VETOR POR UM ESCALAR
vec3 operator*(double t, const vec3 &v)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

vec3 operator*(const vec3 &v, double t)
{
    return t * v;
}

// DIVISÃO DE UM VETOR POR UM ESCALAR
vec3 operator/(const vec3 &v, double t)
{
    return (1 / t) * v;
}

// PRODUTO ESCALAR - DOT PRODUCT
double dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// CROSS PRODUCT
vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// TRANSFORMA EM UM VETOR UNITÁRIO
vec3 unit_vector(const vec3 &v)
{
    return v / v.length();
}

#endif