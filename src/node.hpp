#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>
#include "hittable.hpp"
#include "matrix4.hpp"
#include "rtweekend.hpp" // Já inclui ray.hpp e vec3.hpp de forma limpa

using std::shared_ptr;
using std::vector;

class node : public hittable
{
public:
    node()
    {
        local_matrix = matrix4::identity();
        world_matrix = matrix4::identity();
        inverse_world_matrix = matrix4::identity();
    }

    void add(shared_ptr<hittable> object)
    {
        children.push_back(object);
    }

    void update_transforms(const matrix4 &parent_transform)
    {
        world_matrix = parent_transform * local_matrix;
        inverse_world_matrix = world_matrix.inverse();

        for (auto &child : children)
        {
            auto child_node = std::dynamic_pointer_cast<node>(child);
            if (child_node)
            {
                child_node->update_transforms(world_matrix);
            }
        }
    }

    void translate(const vec3 &offset) { local_matrix = matrix4::translation(offset) * local_matrix; }
    void rotate_y(double degrees) { local_matrix = matrix4::rotation_y(degrees) * local_matrix; }
    void scale(const vec3 &s) { local_matrix = matrix4::scaling(s) * local_matrix; }
    
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // 1. Transforma o raio para o espaço local do nó
        point3 local_origin = inverse_world_matrix.transform_point(r.origin());
        vec3 local_direction = inverse_world_matrix.transform_vector(r.direction());

        // Construtor correto sem o parâmetro .time()
        ray local_ray(local_origin, local_direction);

        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        // 2. Testa a interseção contra as geometrias filhas
        for (const auto &child : children)
        {
            if (child->hit(local_ray, interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        if (hit_anything)
        {
            // 3. Transforma os dados de colisão de volta para o espaço global
            rec.p = world_matrix.transform_point(rec.p);

            // Transposta da inversa para evitar distorções de escala na normal
            vec3 world_normal = inverse_world_matrix.transpose().transform_vector(rec.normal);

            // CORREÇÃO: Passando o raio global 'r' para o cálculo correto da face interna/externa
            rec.set_face_normal(r, world_normal);
        }

        return hit_anything;
    }

private:
    vector<shared_ptr<hittable>> children;
    matrix4 local_matrix;
    matrix4 world_matrix;
    matrix4 inverse_world_matrix;
};

#endif