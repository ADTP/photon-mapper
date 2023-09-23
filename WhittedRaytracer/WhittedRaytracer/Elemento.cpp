#include "Elemento.h"

float Elemento::getAmbiente() {
	return this->ambiente;
}

float Elemento::getDifusa() {
	return this->difusa;
}

float Elemento::getEspecular() {
	return this->especular;
}

float Elemento::getReflexion() {
	return this->reflexion;
}

float Elemento::getRefraccion() {
	return this->refraccion;
}

float Elemento::getTransmision() {
	return this->transmision;
}

vec3 Elemento::getCoeficienteReflexionDifusa() {
	return this->coeficienteReflexionDifusa;
}

vec3 Elemento::getCoeficienteReflexionEspecular() {
	return this->coeficienteReflexionEspecular;
}
