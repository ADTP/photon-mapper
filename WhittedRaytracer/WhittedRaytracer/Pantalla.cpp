#include "Pantalla.h"

Pantalla::Pantalla() {
	this->ancho = 480;
	this->largo = 640;
	this->pixelesPantalla = new vec3 * [ancho];

	for (int i = 0; i < ancho; i++) {
		pixelesPantalla[i] = new vec3[largo];
	}
	this->bitmap = FreeImage_Allocate(ancho, largo, 24);
	this->bitmapReflexion = FreeImage_Allocate(ancho, largo, 24);
	this->bitmapTransmision = FreeImage_Allocate(ancho, largo, 24);
	this->bitmapAmbiente = FreeImage_Allocate(ancho, largo, 24);
	this->bitmapDifuso = FreeImage_Allocate(ancho, largo, 24);
	this->bitmapEspecular = FreeImage_Allocate(ancho, largo, 24);
}

void Pantalla::cargarMalla(Camara camara) {
	//float imageAspectRatio = ancho / (float)largo;
	vec3 centro = camara.posicion +  camara.frente * camara.distanciaCamaraPantalla;

	float d_h = camara.distanciaCamaraPantalla * tan(camara.fov_horizontal / 2 * PI / 180);
	float d_v = camara.distanciaCamaraPantalla * tan(camara.fov_vertical / 2 * PI / 180);

	//vec3 rightFromCenter = centro + camara.derecha * d_h;
	vec3 leftFromCenter = centro - camara.derecha * d_h;
	vec3 upFromCenter = centro + camara.arriba * d_v;
	//vec3 downFromCenter = centro - camara.arriba * d_v;;

	float length_h = 2 * tan(camara.fov_horizontal / 2 * PI / 180);
	float length_v = 2 * tan(camara.fov_vertical / 2 * PI / 180);

	float pixel_width = length_h / ancho;
	float pixel_height = length_v / largo;

	float width_factor = 1;
	float height_factor = 1;

	for (int r = ancho - 1; r >= 0; r--) {
		for (int c = 0; c < largo; c++) {
			pixelesPantalla[r][c] =
				vec3{ leftFromCenter.x + ((width_factor) * (pixel_width / 2)),
				upFromCenter.y - ((height_factor) * (pixel_height / 2)),
				camara.posicion.z - 1 };
			width_factor += 2;
		}
		width_factor = 1;
		height_factor += 2;
	}
}