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
	////bool intersectPlane(const Vec3f & n, const Vec3f & p0, const Vec3f & l0, const Vec3f & l, float& t)

	//// assuming vectors are all normalized
	//float denom = dot(this->normal, rayo->direccion);

	//if (denom > 1e-6) {
	//	vec3 p0l0 = this->origen_normal - rayo->origen;
	//	return dot(p0l0, this->normal) / denom;	// esto seria t, que suplantandolo en la ec de la recta da el punto de interseccion
	//}

	//return 0; // retorna negativo si no hay interseccion

	float direction_ax_by_cz = a * rayo->direccion.x + b * rayo->direccion.y + c * rayo->direccion.z;
	float origin_ax_by_cz_d = a * rayo->origen.x + b * rayo->origen.y + c * rayo->origen.z + d;
	if (direction_ax_by_cz != 0) {
		float t = -origin_ax_by_cz_d / direction_ax_by_cz;
		if (t >= 0) {
			return t;
		}
		else return 0;
	}
	else {
		if (origin_ax_by_cz_d == 0) {
			return 0;
		}
		else {
			return 0;
		}
	}


}

vec3 Plano::normalDelPunto(vec3 punto) {
	return this->origen_normal;
}