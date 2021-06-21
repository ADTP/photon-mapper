#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"

#include "Elemento.h"
#include "Plano.h"

using namespace std;

class Triangulo : public Elemento {
	private:
		vec3 p1;
		vec3 p2;
		vec3 p3;

		Plano* planoPadre;

		float area;

	public:
		Triangulo(
			vec3 p1, vec3 p2, vec3 p3,
			RGBQUAD color, float ambiente, float difusa, float especular,
			float reflexion, float refraccion, float transmision
		);

		~Triangulo();

		float interseccionRayo(Rayo* r);
		vec3 normalDelPunto(vec3 punto);
};

