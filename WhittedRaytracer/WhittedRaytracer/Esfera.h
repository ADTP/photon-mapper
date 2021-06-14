#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"

#include "Elemento.h"

using namespace std;

class Esfera : public Elemento {
	private:

	public:
		Esfera(float radio, float a, float b, float c);
		~Esfera();

		float radio;
		float a;
		float b;
		float c;

		float interseccionRayo(Rayo* rayo);
		vec3 normalDelPunto(vec3 punto);
};

