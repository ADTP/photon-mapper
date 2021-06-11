#include "Escena.h"

Escena::Escena() {
	RGBQUAD color;
	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 1;
	Plano* planoAbajo = new Plano({ 0,0,0 }, { 1,0,0 }, { 0,0,1 }, color);
	color.rgbRed = 20;
	Plano* planoArriba = new Plano({0,400,0}, { 1,400,0 }, { 0,400,1 }, color);
	color.rgbRed = 200;
	Plano* planoIzquierda = new Plano({0,0,0}, {0,1,0}, {0,0,1}, color);
	color.rgbRed = 80;
	Plano* planoDerecha = new Plano({600,0,0}, {600,1,0}, {600,0,1},color);
	color.rgbRed = 150;
	Plano* planoAdelante = new Plano({0,0,500}, {0,1,500}, {1,0,500},color);
	elementos[0] = planoAbajo;
	elementos[1] = planoArriba;
	elementos[2] = planoIzquierda;
	elementos[3] = planoDerecha;
	elementos[4] = planoAdelante;
}

Escena::~Escena() {}
Escena* Escena::instancia = NULL;

Escena* Escena::getInstance() {
	if (instancia == NULL) {
		instancia = new Escena();
	}
	return instancia;
}