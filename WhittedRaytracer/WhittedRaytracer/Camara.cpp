#include "Camara.h"

Camara::Camara() {}

Camara::Camara(vec3 posicion, vec3 normal, vec3 centro, float fov_vertical, float fov_horizontal, float distancia) {
	this->posicion = posicion;

	this->centro = centro;
	this->normal = normal;
	this->derecha = cross(centro, normal);

	this->fov_horizontal = fov_horizontal;
	this->fov_vertical = fov_vertical;

	this->distanciaCamaraPantalla = distancia;
}

Camara::~Camara() {}