#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include <random> 
#include <functional>

#define PI_LUZ 3.14159265358979323846

using namespace glm;

class Luz {
	public:
		Luz(vec3 posicion, vec3 intesidad, RGBQUAD color, int watts, int tipo, float radio, float angulo);
		~Luz();

		vec3 posicion;
		vec3 intesidad;

		int watts;

		RGBQUAD color;

		int tipo;
		float radio;
		float angulo;

		vec3 getPosicionAleatoria();
		bool esDireccionValida(vec3 dir);
};

