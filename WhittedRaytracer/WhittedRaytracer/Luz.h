#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

using namespace glm;

class Luz {

	public:

		Luz(vec3 posicion, vec3 intesidad, RGBQUAD color, float potencia);
		~Luz();

		vec3 posicion;
		vec3 intesidad;

		float potencia;

		RGBQUAD color;
};

