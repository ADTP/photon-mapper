#include "Pantalla.h"

Pantalla* Pantalla::instancia = nullptr;

void Pantalla::setInstance(Pantalla* pantalla) {
	if (instancia == nullptr) {
		instancia = pantalla;
	}
}

Pantalla* Pantalla::getInstance() {
	return instancia;
}

Pantalla::Pantalla(int ancho, int altura) {
	this->ancho = ancho;
	this->altura = altura;
	this->pixelesPantalla = new vec3*[ancho];

	for (int i = 0; i < ancho; i++) {
		pixelesPantalla[i] = new vec3[altura];
	}

	this->bitmapDirecta = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapIndirecta = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapEspecular = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapCausticas = FreeImage_Allocate(ancho, altura, 24);

	this->bitmapResultado = FreeImage_Allocate(ancho, altura, 24);
}

Pantalla::~Pantalla() {}

void Pantalla::cargarMalla(Camara *camara) {
	float aspectRatio = (float)ancho / (float)altura;
	float fov_vertical = camara->fov_horizontal / aspectRatio;

	vec3 centroPantalla = camara->posicion + camara->adelante * camara->distanciaCamaraPantalla;

	float mitadHorizontal = camara->distanciaCamaraPantalla * tan((camara->fov_horizontal / 2) * PI_PANTALLA / 180);
	float mitadVertical = camara->distanciaCamaraPantalla * tan((fov_vertical / 2) * PI_PANTALLA / 180);

	vec3 puntoBordeIzquierda = centroPantalla - camara->derecha * mitadHorizontal;
	vec3 puntoAbajoIzquierda = puntoBordeIzquierda - camara->arriba * mitadVertical;

	float ladoPixel = mitadHorizontal * 2 / ancho;

	for (int y = 0; y < altura; y++) {
		for (int x = 0; x < ancho; x++) {
			pixelesPantalla[x][y] = puntoAbajoIzquierda + camara->derecha * ladoPixel * (float)x;
		}
		puntoAbajoIzquierda = puntoAbajoIzquierda + camara->arriba * ladoPixel;
	}
}
