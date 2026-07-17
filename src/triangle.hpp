#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include "material.hpp"

struct uv_coord
{
    double u, v;
};
class triangle : public hittable
{
public:
    vec3 v0, v1, v2;
    uv_coord uv0, uv1, uv2; // UVs dos vértices
    shared_ptr<material> mat;
    bool has_uv; // Indica se o triângulo tem coordenadas UV

    // Construtor sem textura (mantém compatibilidade)
    triangle(const point3 &a, const point3 &b, const point3 &c, shared_ptr<material> m)
        : v0(a), v1(b), v2(c), mat(m), has_uv(false) {}

    // Novo construtor COM textura
    triangle(const point3 &a, const point3 &b, const point3 &c,
             uv_coord u0, uv_coord u1, uv_coord u2, shared_ptr<material> m)
        : v0(a), v1(b), v2(c), uv0(u0), uv1(u1), uv2(u2), mat(m), has_uv(true) {}

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        const double EPSILON = 1e-8;
        
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        
        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);

        // Se 'a' for muito próximo de 0, o raio é paralelo ao triângulo
        if (a > -EPSILON && a < EPSILON)
            return false;

        double f = 1.0 / a;
        vec3 s = r.origin() - v0;
        double u = f * dot(s, h);

        // O ponto de interseção está fora do triângulo?
        if (u < 0.0 || u > 1.0)
            return false;

        vec3 q = cross(s, edge1);
        double v = f * dot(r.direction(), q);

        // O ponto de interseção está fora do triângulo?
        if (v < 0.0 || u + v > 1.0)
            return false;

        // Neste ponto, sabemos que há interseção. Vamos calcular o 't'
        double t = f * dot(edge2, q);

        if (!ray_t.surrounds(t))
            return false; // O triângulo está atrás da câmera ou além da distância máxima

        rec.t = t;
        rec.p = r.at(rec.t);
        rec.mat = mat;

        // --- CÁLCULO DAS UVs ---
        if (has_uv)
        {
            double w = 1.0 - u - v; // Coordenada baricêntrica do v0
            rec.u = (w * uv0.u) + (u * uv1.u) + (v * uv2.u);
            rec.v = (w * uv0.v) + (u * uv1.v) + (v * uv2.v);
        }
        else
        {
            rec.u = 0;
            rec.v = 0;
        }

        // A normal do triângulo é perpendicular aos seus dois lados
        vec3 outward_normal = unit_vector(cross(edge1, edge2));
        rec.set_face_normal(r, outward_normal);

        return true;
    }
};

#endif