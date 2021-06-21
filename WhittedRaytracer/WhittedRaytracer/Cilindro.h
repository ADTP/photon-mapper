#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"

#include "Elemento.h"

using namespace std;

class Cilindro : public Elemento {
	public:

		Cilindro(
			vec3 posicion, float radio, float altura,
			RGBQUAD color, float ambiente, float difusa, float especular,
			float reflexion, float refraccion, float transmision
		);

		~Cilindro();

		float radio;
		float altura;

		float interseccionRayo(Rayo* rayo);
		vec3 normalDelPunto(vec3 punto);
};