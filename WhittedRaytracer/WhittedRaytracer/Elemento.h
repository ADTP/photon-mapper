#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include <string>

using namespace glm;

class Elemento {
	public:
		Elemento(std::string nombreArchivo, RGBQUAD color, vec3 posicion, vec3 coeficienteReflexionDifusa, vec3 coeficienteReflexionEspecular);
		~Elemento();

		std::string nombreArchivo;

		RGBQUAD color;
		
		vec3 posicion;
		
		vec3 coeficienteReflexionDifusa;
		vec3 coeficienteReflexionEspecular;

		/*
		float ambiente;
		float difusa;
		float especular;

		float reflexion;
		float refraccion;
		float transmision;
		*/
};
