#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include "material.hpp"

class sphere : public hittable
{
public:
    sphere(const point3 &center, double radius, shared_ptr<material> m)
        : center(center), radius(std::fmax(0, radius)), mat(m) {}

    // hittable
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();           // vetor da origem ate o centro
        auto a = r.direction().length_squared(); // coeficientes da equação
        auto h = dot(r.direction(), oc); // d * (C - Q)
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c; // discriminante da equação
        if (discriminant < 0)              // nao atingiu a esfera, sem raizes reais
            return false;

        auto sqrtd = std::sqrt(discriminant); // raiz do discriminate, o delta

        auto root = (h - sqrtd) / a; // Calcula a menor raiz (mais próxima da câmera)
        if (!ray_t.surrounds(root)) // Verifica se essa raiz está dentro do intervalo válido de t
        {
            root = (h + sqrtd) / a; // Se a menor não serve, tenta a segunda raiz (mais distante)
            if (!ray_t.surrounds(root)) // Se nenhuma das duas está no intervalo:
                return false; // Não houve hit
        }

        rec.t = root;  // t é o parâmetro do raio na equação, escalar
        rec.p = r.at(rec.t); // calculo do ponto exato no espaço onde o raio atingiu a esfera
        vec3 outward_normal = (rec.p - center) / radius; // calculo da normal da esfera no ponto de contato
        // hittable
        rec.set_face_normal(r, outward_normal); //Garante que a normal sempre aponte contra o raio

        vec3 p = (rec.p - center) / radius; // Ponto unitário na esfera
        auto phi = std::atan2(-p.z(), p.x()) + pi;
        auto theta = std::acos(-p.y());
        rec.u = phi / (2.0 * pi);
        rec.v = theta / pi;

        rec.mat = mat;
        return true;
    }

private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif