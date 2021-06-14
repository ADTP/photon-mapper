#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include "Rayo.h"

using namespace glm;

class Elemento {
	public:

		float difusa;
		float ambiente;
		float especular;

		RGBQUAD color;

		vec3 posicion;

		virtual float interseccionRayo(Rayo* rayo) = 0;
		virtual vec3 normalDelPunto(vec3 punto) = 0;
};
