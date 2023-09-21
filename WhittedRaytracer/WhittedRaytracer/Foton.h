#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
using namespace glm;

class Foton {
	public:
		Foton(vec3 posicion, RGBQUAD potencia, char phi, char theta, short flag);
		~Foton();

		vec3 posicion; // position
		RGBQUAD potencia; // power packed as 4 chars [Ward91]
		char phi, theta; // compressed incident direction
		short flag;
};
