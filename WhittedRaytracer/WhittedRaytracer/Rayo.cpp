#include "Rayo.h"

Rayo::Rayo(vec3 origen, vec3 destino, float refraccionObjetoActual) {
	this->origen = origen;
	this->direccion = normalize(destino - origen);

	this->refraccionObjetoActual = refraccionObjetoActual;
}

Rayo::Rayo(vec3 origen, vec3 destino, float refraccionObjetoActual, int j, int i) {
	this->origen = origen;
	this->direccion = normalize(destino - origen);

	this->refraccionObjetoActual = refraccionObjetoActual;

	this->iteradorAncho = j;
	this->iteradorAltura = i;
}

Rayo::~Rayo() {}