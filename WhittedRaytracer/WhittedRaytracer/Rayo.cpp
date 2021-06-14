#include "Rayo.h"

Rayo::Rayo(vec3 origen, vec3 direccion) {
	this->origen = origen;
	this->direccion = normalize(direccion - origen);
}

Rayo::~Rayo() {}