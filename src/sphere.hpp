#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "rtweekend.hpp"

class sphere : public hittable
{
public:
    sphere(const point3 &center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();           // vetor da origem ate o centro
        auto a = r.direction().length_squared(); // coeficientes da equação
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c; // discriminante da equação
        if (discriminant < 0)              // nao atingiu a esfera
            return false;

        auto sqrtd = std::sqrt(discriminant);

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
        rec.set_face_normal(r, outward_normal); //Garante que a normal sempre aponte contra o raio

        return true;
    }

private:
    point3 center;
    double radius;
};

#endif