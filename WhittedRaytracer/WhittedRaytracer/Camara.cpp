#include "Camara.h"

Camara::Camara(vec3 posicion, vec3 arriba, vec3 adelante, float fov_horizontal, float distanciaCamaraPantalla) {
	this->posicion = posicion;

	this->arriba = normalize(arriba);
	this->adelante = normalize(adelante);
	this->derecha = normalize(cross(adelante, arriba));

	this->fov_horizontal = fov_horizontal;

	this->distanciaCamaraPantalla = distanciaCamaraPantalla;
}

Camara* Camara::instancia = nullptr;

Camara* Camara::getInstance() {
	return instancia;
}

void Camara::setInstance(Camara* camara) {
	if (instancia == nullptr) {
		instancia = camara;
	}
}

Camara::~Camara() {}