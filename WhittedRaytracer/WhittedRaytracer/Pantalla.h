#pragma once

#include "../Freeimage/FreeImage.h"
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "Camara.h"

#define PI 3.14159265

using namespace glm;

class Pantalla {
	private:
		static Pantalla* instancia;

	public:
		Pantalla();
		~Pantalla();
		
		static Pantalla* getInstance();

		void cargarMalla(Camara *camara);

		int ancho;
		int altura;

		vec3** pixelesPantalla;

		FIBITMAP* bitmap;

		FIBITMAP* bitmapAmbiente;
		FIBITMAP* bitmapDifuso;
		FIBITMAP* bitmapEspecular;

		FIBITMAP* bitmapReflexion;
		FIBITMAP* bitmapTransmision;
		
};
