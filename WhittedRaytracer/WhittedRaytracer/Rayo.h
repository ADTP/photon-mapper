#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
using namespace glm;

class Rayo {
	public:
		Rayo(vec3 origen, vec3 direccion);
		~Rayo();

		vec3 origen;
		vec3 direccion;
};

