#include "Rayo.h"

Rayo::Rayo(vec3 origen, vec3 destino) {
	this->origen = origen;
	this->direccion = normalize(destino - origen);
}

Rayo::~Rayo() {}