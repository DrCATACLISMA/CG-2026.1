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

        // PERGUNTAR PARA O RENER
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
};

#endif