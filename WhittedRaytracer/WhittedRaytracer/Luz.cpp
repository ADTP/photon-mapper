#include "Luz.h"

Luz::Luz(vec3 posicion, vec3 intesidad, RGBQUAD color) {
	this->posicion = posicion;
	this->intesidad = intesidad;
	this->color = color;
}

Luz::~Luz() {};