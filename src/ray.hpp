#ifndef RAY_H
#define RAY_H

#include "vector.hpp"

class ray {
	public:
		ray() {}

		// construtor...
		// origin: ponto de origem do raio
		// direction: direção do raio
		// IMPORTANTE !!!
		/*
		const: não pode modificar
		& passa o endereço, NÃO A COPIA
		*/ 
		ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}
		// INICIALIZAÇÃO
		/*
			EQUIVALENTE EM C:
			struct ray create_ray(struct vec3 origin, struct vec3 direction) {
				struct ray r;
				r.orig = origin;
				r.dir  = direction;
				return r;
			}
		*/

		// GETTERS
		const point3& origin() const  { return orig; }
		const vec3& direction() const { return dir; }

		/*
		t = 0 → ponto inicial
		t = 1 → anda 1 passo na direção
		t = 2 → anda 2 passos
		t < 0 → anda para trás
		*/
	
		point3 at(double t) const {
			return orig + t*dir;
		}

	private:
		point3 orig;
		vec3 dir;
};

#endif