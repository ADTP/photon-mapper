#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
using namespace glm;

class Rayo {
	public:
		Rayo(vec3 origen, vec3 destino, float refraccionObjetoActual);
		~Rayo();

		vec3 origen;
		vec3 destino;
		vec3 direccion;

		float refraccionObjetoActual;

		Rayo(vec3 origen, vec3 destino, float refraccionObjetoActual, int j, int i);
		int iteradorAncho;
		int iteradorAltura;
};
