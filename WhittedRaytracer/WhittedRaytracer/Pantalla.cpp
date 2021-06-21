#include "Pantalla.h"

Pantalla* Pantalla::instancia = nullptr;

Pantalla* Pantalla::getInstance() {
	if (instancia == nullptr) {
		instancia = new Pantalla();
	}
	return instancia;
}

Pantalla::Pantalla() {
	this->ancho = 640;
	this->altura = 480;
	this->pixelesPantalla = new vec3*[altura];

	for (int i = 0; i < altura; i++) {
		pixelesPantalla[i] = new vec3[ancho];
	}

	this->bitmap = FreeImage_Allocate(ancho, altura, 24);

	this->bitmapAmbiente = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapDifuso = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapEspecular = FreeImage_Allocate(ancho, altura, 24);

	this->bitmapReflexion = FreeImage_Allocate(ancho, altura, 24);
	this->bitmapTransmision = FreeImage_Allocate(ancho, altura, 24);
}

Pantalla::~Pantalla() {}

void Pantalla::cargarMalla(Camara *camara) {
	vec3 centroPantalla = camara->posicion + camara->adelante * camara->distanciaCamaraPantalla;

	float mitadHorizontal = camara->distanciaCamaraPantalla * tan((camara->fov_horizontal / 2) * PI / 180);
	float mitadVertical = camara->distanciaCamaraPantalla * tan((camara->fov_vertical / 2) * PI / 180);

	vec3 puntoBordeIzquierda = centroPantalla - camara->derecha * mitadHorizontal;
	vec3 puntoBordeAbajo = centroPantalla - camara->arriba * mitadVertical;
	vec3 puntoBordeArriba = centroPantalla + camara->arriba * mitadVertical;

	float anchoPixel = mitadHorizontal * 2 / ancho;
	float alturaPixel = mitadVertical * 2 / altura;

	/*float anchoPixel = 2 * tan((camara->fov_horizontal / 2) * PI / 180);
	float alturaPixel = 2 * tan((camara->fov_vertical / 2) * PI / 180);*/

	float iteradorAncho = 1;
	float iteradorAltura = 1;

	for (int i = altura - 1; i >= 0; i--) {
		for (int j = 0; j < ancho; j++) {
			pixelesPantalla[i][j] = {
				puntoBordeIzquierda.x + ((iteradorAncho) * (anchoPixel / 2)),
				puntoBordeArriba.y - ((iteradorAltura) * (alturaPixel / 2)),
				camara->posicion.z - camara->distanciaCamaraPantalla
			};

			//printf("x:%f y:%f z:%f \n", pixelesPantalla[i][j].x, pixelesPantalla[i][j].y, pixelesPantalla[i][j].z);

			iteradorAncho += 2;
		}

		iteradorAncho = 1;
		iteradorAltura += 2;
	}
}