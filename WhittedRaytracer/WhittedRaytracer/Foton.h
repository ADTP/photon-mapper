#pragma once

#include "../glm-0.9.9.8/glm-0.9.9.8/glm/glm.hpp"
#include "../Freeimage/FreeImage.h"

#include <iostream>
#include <fstream>

using namespace glm;

class Foton {
	public:
		Foton(vec3 posicion, RGBQUAD potencia, char phi, char theta, short flag);
		~Foton();

		vec3 posicion; // position
		RGBQUAD potencia; // power packed as 4 chars [Ward91]
		char phi, theta; // compressed incident direction
		short flag;

		/*static const int DIM = 3;

		float& operator[](int index) {
			return posicion[index];
		}

		const float& operator[](int index) const {
			return posicion[index];
		}*/

		Foton();

		void serializar(std::ostream& stream) const {
			stream.write(reinterpret_cast<const char*>(&posicion), sizeof(vec3));
			stream.write(reinterpret_cast<const char*>(&potencia), sizeof(RGBQUAD));
		}

		void deserializar(std::istream& stream) {
			stream.read(reinterpret_cast<char*>(&posicion), sizeof(vec3));
			stream.read(reinterpret_cast<char*>(&potencia), sizeof(RGBQUAD));
		}
};