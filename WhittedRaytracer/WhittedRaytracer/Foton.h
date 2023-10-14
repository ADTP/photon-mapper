#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include <iostream>
#include <fstream>

using namespace glm;

class Foton {
	public:
		Foton(vec3 posicion, vec3 potencia, vec3 direccionIncidente, char phi, char theta, short flag);
		~Foton();

		vec3 posicion; // position
		vec3 potencia; // power packed as 4 chars [Ward91]
		char phi, theta; // compressed incident direction
		short flag;

		vec3 direccionIncidente;

		Foton();

		void serializar(std::ostream& stream) const {
			stream.write(reinterpret_cast<const char*>(&posicion), sizeof(vec3));
			stream.write(reinterpret_cast<const char*>(&potencia), sizeof(vec3));
			stream.write(reinterpret_cast<const char*>(&direccionIncidente), sizeof(vec3));
		}

		void deserializar(std::istream& stream) {
			stream.read(reinterpret_cast<char*>(&posicion), sizeof(vec3));
			stream.read(reinterpret_cast<char*>(&potencia), sizeof(vec3));
			stream.read(reinterpret_cast<char*>(&direccionIncidente), sizeof(vec3));
		}
};