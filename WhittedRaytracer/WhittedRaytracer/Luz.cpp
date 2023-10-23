#include "Luz.h"

Luz::Luz(vec3 posicion, vec3 intesidad, RGBQUAD color, int watts, int tipo, float radio, float angulo) {
	this->posicion = posicion;
	this->intesidad = intesidad;
	this->color = color;
	this->watts = watts;
	this->tipo = tipo;
	this->radio = radio;
	this->angulo = angulo;
}

Luz::~Luz() {}

vec3 Luz::getPosicionAleatoria() {
	std::mt19937 engine;  //Mersenne twister MT19937
	std::uniform_real_distribution<double> distribution2(-1.0, 1.0);
	static auto generator2 = std::bind(distribution2, engine);

	switch (this->tipo) {
	
	case 0: // puntual
		return this->posicion;
	
	case 1: // esfera
		vec3 puntoEsfera;
		
		do {
			puntoEsfera.x = generator2() * this->radio;
			puntoEsfera.y = generator2() * this->radio;
			puntoEsfera.z = generator2() * this->radio;
		} while (pow(puntoEsfera.x, 2) + pow(puntoEsfera.y, 2) + pow(puntoEsfera.z, 2) > this->radio);

		return this->posicion + puntoEsfera;
	
	case 2: // disco
		vec3 puntoDisco;
		
		do {
			puntoDisco.x = generator2() * this->radio;
			puntoDisco.y = 0;
			puntoDisco.z = generator2() * this->radio;
		} while (pow(puntoDisco.x, 2) + pow(puntoDisco.y, 2) + pow(puntoDisco.z, 2) > this->radio);

		return this->posicion + puntoDisco;
	}
}

bool Luz::esDireccionValida(vec3 dir) {
	if (this->tipo != 2) { return true; }

	float productoPunto = dot(normalize(dir), { 0, 1, 0 });
	
	return productoPunto > cos(this->angulo * PI_LUZ / 180);
}