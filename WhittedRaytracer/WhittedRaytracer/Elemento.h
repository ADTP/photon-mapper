#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include "Rayo.h"

using namespace glm;

class Elemento {
	public:

		float ambiente;
		float difusa;
		float especular;

		float reflexion;
		float refraccion;
		float transmision;

		RGBQUAD color;

		vec3 posicion;

		virtual float interseccionRayo(Rayo* rayo) = 0;
		virtual vec3 normalDelPunto(vec3 punto) = 0;

		float getAmbiente();
		float getDifusa();
		float getEspecular();

		float getReflexion();
		float getRefraccion();
		float getTransmision();
};
