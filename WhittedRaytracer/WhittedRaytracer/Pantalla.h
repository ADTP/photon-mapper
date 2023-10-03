#pragma once

#include "../Freeimage/FreeImage.h"
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "Camara.h"

#define PI_PANTALLA 3.14159265358979323846

using namespace glm;

class Pantalla {
	private:
		static Pantalla* instancia;

	public:
		Pantalla(int ancho, int altura);
		~Pantalla();
		
		static Pantalla* getInstance();
		static void setInstance(Pantalla* pantalla);

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
