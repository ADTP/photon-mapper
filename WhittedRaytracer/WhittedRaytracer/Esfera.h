#pragma once
#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"
#include <embree3/rtcore.h>

using namespace glm;

class Esfera
{
public:
	Esfera(vec3 posicion, float radio);
	
	vec3 posicion;
	float radio;

	~Esfera();
};

