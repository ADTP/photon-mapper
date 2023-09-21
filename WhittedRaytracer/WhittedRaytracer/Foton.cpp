#include "Foton.h"
#include "../Freeimage/FreeImage.h"

Foton::Foton(vec3 posicion, RGBQUAD potencia, char phi, char theta, short flag) {
	this->posicion = posicion;
	this->potencia = potencia;
	this->phi = phi;
	this->theta = theta;
	this->flag = flag;
}

Foton::~Foton() {}