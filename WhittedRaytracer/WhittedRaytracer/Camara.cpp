#include "Camara.h"

Camara::Camara() {}

Camara::Camara(vec3 posicion, vec3 normal, vec3 centro, float lejos, float cerca, float fov_vertical, float fov_horizontal) {
	this->centro = centro;
	this->posicion = posicion;
	this->cerca = cerca;
	this->lejos = lejos;
	this->normal = normal;
	this->fov_horizontal = fov_horizontal;
	this->fov_vertical = fov_vertical;
}

Camara::~Camara() {}