#include "Elemento.h"

Elemento::Elemento(std::string nombreArchivo, RGBQUAD color, vec3 posicion, vec3 coeficienteReflexionDifusa, vec3 coeficienteReflexionEspecular) {
	this->nombreArchivo = nombreArchivo;
	
	this->color = color;
	this->posicion = posicion;
	
	this->coeficienteReflexionDifusa = coeficienteReflexionDifusa;
	this->coeficienteReflexionEspecular = coeficienteReflexionEspecular;
}

Elemento::~Elemento() {}