#include "Luz.h"

Luz::Luz(vec3 posicion, vec3 intesidad, RGBQUAD color, int watts) {
	this->posicion = posicion;
	this->intesidad = intesidad;
	this->color = color;
	this->watts = watts;
}

Luz::~Luz() {};