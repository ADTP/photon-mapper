#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"

#include "Elemento.h"

using namespace std;

class Esfera : public Elemento {
	public:

		Esfera(
			float radio, float a, float b, float c, 
			RGBQUAD color, float ambiente, float difusa, float especular,
			float reflexion, float refraccion, float transmision,
			vec3 coeficienteReflexionDifusa,
			vec3 coeficienteReflexionEspecular
		);

		~Esfera();

		float radio;

		float interseccionRayo(Rayo* rayo);
		vec3 normalDelPunto(vec3 punto);
};

