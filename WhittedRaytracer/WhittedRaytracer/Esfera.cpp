#include "Esfera.h"

Esfera::Esfera(vec3 posicion, float radio) {
	this->posicion = posicion;
	this->radio = radio;
}

Esfera::~Esfera()
{
}

void Esfera::sphereBoundsFunction(const struct RTCBoundsFunctionArguments args)
{
	RTCBounds bounds_o = *args.bounds_o;
	bounds_o.lower_x = this->posicion.x - this->radio;
	bounds_o.lower_y = this->posicion.y - this->radio;
	bounds_o.lower_z = this->posicion.z - this->radio;
	bounds_o.upper_x = this->posicion.x + this->radio;
	bounds_o.upper_y = this->posicion.y + this->radio;
	bounds_o.upper_z = this->posicion.z + this->radio;
}
