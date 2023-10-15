#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include <string>

using namespace glm;

class Elemento {
	public:
		Elemento(std::string nombreArchivo, RGBQUAD color, vec3 posicion, vec3 coeficienteReflexionDifusa, vec3 coeficienteReflexionEspecular, float indiceRefraccion, float radio);
		~Elemento();

		std::string nombreArchivo;

		RGBQUAD color;
		
		vec3 posicion;
		
		vec3 coeficienteReflexionDifusa;
		vec3 coeficienteReflexionEspecular;

		float indiceRefraccion;
		float radio;
};
