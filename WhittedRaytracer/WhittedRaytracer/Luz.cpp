#include "Luz.h"

Luz::Luz(vec3 posicion, vec3 intesidad, RGBQUAD color, float potencia) {
	this->posicion = posicion;
	this->intesidad = intesidad;
	this->color = color;
	this->potencia = potencia;
}

Luz::~Luz() {};