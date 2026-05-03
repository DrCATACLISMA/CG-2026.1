#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include <vector>

class hittable_list : public hittable // armazena uma lista de objetos que podem ser atingidos por um raio
{
public:
    std::vector<shared_ptr<hittable>> objects; // varios objetos em cena, esferas, planos etc

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); } 
    /*
        shared_ptr<type>é um ponteiro para algum tipo alocado
        com semântica de contagem de referências.
        incrementa quando é atribuido um valor 
        decrementa à medida que os ponteiros compartilhados saem do escopo
    */

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override // verifica se um raio bateu em algum objeto da lista
    {
        hit_record temp_rec; // guarda hits temporários
        bool hit_anything = false; // flag se encontrou algo
        auto closest_so_far = ray_t.max; // controla o objeto mais próximo

        for (const auto &object : objects) // loop de objetos
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) // teste de interseção
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;

                // atualiza as variaveis    
            }
        }

        return hit_anything;
    }
};

#endif