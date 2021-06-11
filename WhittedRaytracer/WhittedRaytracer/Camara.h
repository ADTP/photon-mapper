#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
using namespace glm;

class Camara
{
public:
	vec3 posicion;

	vec3 normal;
	vec3 centro;
	vec3 derecha;

	float lejos;
	float cerca;
	float fov_vertical;
	float fov_horizontal;
	float distanciaCamaraPantalla;

	Camara();
	~Camara();
	Camara(vec3 posicion, vec3 normal, vec3 centro, float fov_vertical, float fov_horizontal, float distancia);

};

