#include "Camara.h"

Camara::Camara(vec3 posicion, vec3 arriba, vec3 adelante, float fov_vertical, float fov_horizontal, float distanciaCamaraPantalla) {
	this->posicion = posicion;

	this->arriba = normalize(arriba);
	this->adelante = normalize(adelante);
	this->derecha = normalize(cross(adelante, arriba));

	this->fov_horizontal = fov_horizontal;
	this->fov_vertical = fov_vertical;

	this->distanciaCamaraPantalla = distanciaCamaraPantalla;
}

Camara::~Camara() {}