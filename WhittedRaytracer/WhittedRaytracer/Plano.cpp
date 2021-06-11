#include "Plano.h"


Plano::Plano(vec3 p1, vec3 p2, vec3 p3, RGBQUAD color) {
	this->origen_normal = p1;

	vec3 p1p2 = p1 - p2;
	vec3 p1p3 = p1 - p3;

	this->normal = normalize(cross(p1p2, p1p3));

	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -(a * p1.x + b * p1.y + c * p1.z);

	this->color = color;

	this->difusa = 1;
	this->especular = 1;
	this->ambiente = 1;
}

float Plano::interseccionRayo(Rayo *rayo) {
	//bool intersectPlane(const Vec3f & n, const Vec3f & p0, const Vec3f & l0, const Vec3f & l, float& t)

	// assuming vectors are all normalized
	float denom = dot(this->normal, rayo->direccion);

	if (denom > 1e-6) {
		vec3 p0l0 = this->origen_normal - rayo->origen;
		return dot(p0l0, this->normal) / denom;	// esto seria t, que suplantandolo en la ec de la recta da el punto de interseccion
	}

	return -1; // retorna negativo si no hay interseccion
}

vec3 Plano::normalDelPunto(vec3 punto) {
	return this->origen_normal;
}