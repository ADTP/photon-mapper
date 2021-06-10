#include "Rayo.h"

Rayo::Rayo() {}

Rayo::~Rayo() {}

Rayo::Rayo(vec3 origen, vec3 direccion) {
	this->origen = origen;
	this->direccion = normalize(direccion);
}
