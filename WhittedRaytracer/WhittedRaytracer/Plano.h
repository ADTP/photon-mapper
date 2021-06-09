#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "Rayo.h"
using namespace glm;

class Plano
{
private:
	float a;
	float b;
	float c;
	float d;
	float r;
	float g;
	float b;
	float difusa;
	float ambiente;
	float especular;
public:
	Plano();
	~Plano();
	Plano(vec3 p1, vec3 p2, vec3 p3, float r, float g, float b);
	void interseccionRayo(Rayo r);
};

