#include "Escena.h"

Escena::Escena() {
	RGBQUAD color;

	/*color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Plano* planoAbajo = new Plano({ 0,0,0 }, { 1,0,0 }, { 0,0,1 }, color);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 100;
	Plano* planoArriba = new Plano({0,10,0}, { 1,10,0 }, { 0,10,1 }, color);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 255;
	Plano* planoIzquierda = new Plano({0,0,0}, {0,1,0}, {0,0,1}, color);

	color.rgbRed = 0;
	color.rgbGreen = 255;
	color.rgbBlue = 0;
	Plano* planoDerecha = new Plano({10,0,0}, {10,1,0}, {10,0,1},color);*/

	color.rgbRed = 255;
	color.rgbGreen = 0;
	color.rgbBlue = 0;
	Plano* planoAdelante = new Plano({0,0,10}, {0,1,10}, {1,0,10},color);

	//elementos.push_back(planoAbajo);
	//elementos.push_back(planoArriba);
	//elementos.push_back(planoIzquierda);
	//elementos.push_back(planoDerecha);
	elementos.push_back(planoAdelante);
}

Escena::~Escena() {}
Escena* Escena::instancia = NULL;

Escena* Escena::getInstance() {
	if (instancia == NULL) {
		instancia = new Escena();
	}
	return instancia;
}