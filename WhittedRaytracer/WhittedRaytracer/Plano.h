#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "Rayo.h"
#include "Elemento.h";
#include "../Freeimage/FreeImage.h"

using namespace glm;

class Plano : public Elemento {
	private:
		float a;
		float b;
		float c;
		float d;

		vec3 origen_normal;
		vec3 normal;

	public:
		Plano(vec3 p1, vec3 p2, vec3 p3, RGBQUAD color);
		~Plano();

		float interseccionRayo(Rayo *r);
		vec3 normalDelPunto(vec3 punto);
};

