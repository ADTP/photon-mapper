#include "Triangulo.h"

float calcularArea(vec3 p1, vec3 p2, vec3 p3) {
	float p1p2 = distance(p1, p2);
	float p2p3 = distance(p2, p3);
	float p3p1 = distance(p3, p1);

	float perimetro = p1p2 + p2p3 + p3p1;
	float sp = perimetro / 2;

	return sqrt(sp * (sp - p1p2) * (sp - p2p3) * (sp - p3p1));
}

Triangulo::Triangulo(vec3 p1, vec3 p2, vec3 p3,
	RGBQUAD color, float ambiente, float difusa, float especular,
	float reflexion, float refraccion, float transmision) {

	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;

	this->planoPadre = new Plano(p1, p2, p3, { 0,0,0 }, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, { 0, 0, 0 }, { 0, 0, 0 });

	this->area = calcularArea(p1, p2, p3);

	this->color = color;

	this->ambiente = ambiente;
	this->difusa = difusa;
	this->especular = especular;

	this->reflexion = reflexion;
	this->refraccion = refraccion;
	this->transmision = transmision;
}

float Triangulo::interseccionRayo(Rayo* rayo) {
	float tPlano = this->planoPadre->interseccionRayo(rayo);
	vec3 interseccion = rayo->origen + tPlano * rayo->direccion;

	float sumaTotal = calcularArea(p1, p2, interseccion) + calcularArea(p2, p3, interseccion) + calcularArea(p3, p1, interseccion);

	if (sumaTotal > this->area - 0.0009 && sumaTotal < this->area + 0.0009) {
		return tPlano;
	}

	return 0;
}

vec3 Triangulo::normalDelPunto(vec3 punto) {
	vec3 res = -this->planoPadre->normalDelPunto(punto);
	return res;
}

Triangulo::~Triangulo() {
	delete(this->planoPadre);
}