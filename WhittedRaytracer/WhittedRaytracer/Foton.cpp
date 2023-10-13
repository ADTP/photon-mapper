#include "Foton.h"
#include "../Freeimage/FreeImage.h"

Foton::Foton(vec3 posicion, vec3 potencia, vec3 direccionIncidente, char phi, char theta, short flag) {
	this->posicion = posicion;
	this->potencia = potencia;
	this->direccionIncidente = direccionIncidente;
	this->phi = phi;
	this->theta = theta;
	this->flag = flag;
}

Foton::Foton() {}

Foton::~Foton() {}