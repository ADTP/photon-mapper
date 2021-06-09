#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
using namespace glm;

class Camara
{
public:
	vec3 posicion;
	vec3 normal;
	vec3 centro;
	double lejos;
	double cerca;
	double fov_vertical;
	double fov_horizontal;
	Camara();
	~Camara();
	Camara(vec3 posicion, vec3 normal, vec3 centro, double lejos, double cerca, double fov_vertical, double fov_horizontal);

};

