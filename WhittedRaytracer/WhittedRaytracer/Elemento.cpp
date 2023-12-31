#include "Elemento.h"

Elemento::Elemento(std::string nombreArchivo, RGBQUAD color, vec3 posicion, vec3 coeficienteReflexionDifusa, vec3 coeficienteReflexionEspecular, float indiceRefraccion, float radio) {
	this->nombreArchivo = nombreArchivo;
	
	this->color = color;
	this->posicion = posicion;
	
	this->coeficienteReflexionDifusa = coeficienteReflexionDifusa;
	this->coeficienteReflexionEspecular = coeficienteReflexionEspecular;

	this->indiceRefraccion = indiceRefraccion;
	this->radio = radio;
}

Elemento::~Elemento() {}