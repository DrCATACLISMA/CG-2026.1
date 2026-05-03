#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.hpp"

class hit_record
{
public:
  point3 p;
  vec3 normal;
  double t;
  bool front_face;

  void set_face_normal(const ray &r, const vec3 &outward_normal) // garante que a normal da superfície sempre aponte contra o raio
  {
    // Define a normal quando acerta a esfera
    // IMPORTANTE: o `outward_normal` é unitário

    front_face = dot(r.direction(), outward_normal) < 0; 
    /*
      detectar de que lado o raio veio
        se < 0 → ângulo > 90° → vetores em sentidos opostos
        se > 0 → vetores “apontam parecido”
    */
    normal = front_face ? outward_normal : -outward_normal; // ajuste da normal
    /*
      Se o raio veio de fora:
        usa a normal normal (apontando pra fora)
      Se veio de dentro:
        inverte a normal
    */
  }
};

class hittable
{
public:
  virtual ~hittable() = default;

  virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;
};

#endif