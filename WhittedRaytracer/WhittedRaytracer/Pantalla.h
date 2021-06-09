#pragma once
#include "../Freeimage/FreeImage.h"
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "Camara.h"
#define PI 3.14159265

using namespace glm;

class Pantalla
{
public:
	Pantalla();
	~Pantalla();
	void cargarMalla(Camara camara);
	int ancho;
	int largo;
	FIBITMAP* bitmap;
	FIBITMAP* bitmapReflexion;
	FIBITMAP* bitmapTransmision;
	FIBITMAP* bitmapAmbiente;
	FIBITMAP* bitmapDifuso;
	FIBITMAP* bitmapEspecular;
	vec3** pixelesPantalla;
};

