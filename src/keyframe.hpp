#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "rtweekend.hpp"
#include <vector>
#include <algorithm>

/*
    SISTEMA DE KEYFRAMES
    ---------------------
    Ideia: em vez de calcular a posição de um objeto a cada frame com uma
    fórmula fechada, definimos apenas algumas POSES-CHAVE (keyframes) em
    instantes de tempo específicos (ex: "no chão em t=0s", "no topo em t=0.78s").

    Para qualquer frame que caia ENTRE dois keyframes, o valor é calculado
    por INTERPOLAÇÃO (tweening) entre a pose anterior e a próxima.

    Isso é a base de qualquer animação em softwares 3D (Blender, Maya, etc):
    o artista só define as poses importantes, o computador preenche o resto.
*/

enum class easing_type
{
    LINEAR,     // velocidade constante entre os keyframes
    EASE_IN,    // começa devagar, termina rápido (ex: objeto caindo por gravidade)
    EASE_OUT,   // começa rápido, termina devagar (ex: objeto subindo, perdendo velocidade)
    EASE_IN_OUT // suave nas duas pontas
};

struct keyframe
{
    double time;     // instante de tempo (segundos) em que esta pose acontece
    point3 position; // posição do objeto nesse instante
    easing_type easing = easing_type::LINEAR; // como interpolar A PARTIR deste keyframe até o próximo
};

// --- Funções de easing: recebem u em [0,1] (progresso bruto/linear) e devolvem u' suavizado ---
inline double ease_apply(easing_type type, double u)
{
    switch (type)
    {
    case easing_type::EASE_IN:
        return u * u; // acelera (bom para simular queda por gravidade)
    case easing_type::EASE_OUT:
        return 1.0 - (1.0 - u) * (1.0 - u); // desacelera (bom para simular subida perdendo força)
    case easing_type::EASE_IN_OUT:
        return (u < 0.5) ? (2.0 * u * u) : (1.0 - std::pow(-2.0 * u + 2.0, 2) / 2.0);
    case easing_type::LINEAR:
    default:
        return u;
    }
}

/*
    Recebe a lista de keyframes (DEVE estar ordenada por tempo crescente)
    e o instante de tempo desejado `t`, devolve a posição interpolada.
*/
inline point3 keyframe_interpolate(const std::vector<keyframe> &kfs, double t)
{
    if (kfs.empty())
        return point3(0, 0, 0);

    // Antes do primeiro ou depois do último keyframe: mantém a pose extrema (clamp)
    if (t <= kfs.front().time)
        return kfs.front().position;
    if (t >= kfs.back().time)
        return kfs.back().position;

    // Encontra o par de keyframes (k0, k1) que envolve o tempo t
    for (size_t i = 0; i + 1 < kfs.size(); i++)
    {
        const keyframe &k0 = kfs[i];
        const keyframe &k1 = kfs[i + 1];

        if (t >= k0.time && t <= k1.time)
        {
            double u = (t - k0.time) / (k1.time - k0.time); // progresso linear (0..1) dentro do trecho
            double u_suave = ease_apply(k0.easing, u);       // aplica a suavização definida no keyframe de partida

            // interpolação linear (lerp) componente a componente, usando o progresso já suavizado
            return k0.position + u_suave * (k1.position - k0.position);
        }
    }

    return kfs.back().position; // segurança (não deveria chegar aqui)
}

#endif
