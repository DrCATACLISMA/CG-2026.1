#ifndef RAY_H
#define RAY_H

#include "vector.hpp"

class ray {
	public:
		ray() {}

		ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

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