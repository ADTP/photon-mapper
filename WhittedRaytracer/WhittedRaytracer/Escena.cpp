#include "Escena.h"
#include "Esfera.h"

Escena::Escena() {
	RGBQUAD color;

	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Plano* planoAbajo = new Plano({0, 0, 0}, {0, 0, 1}, {1, 0, 0}, color);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 100;
	Plano* planoArriba = new Plano({0, 6, 0}, {1, 6, 0}, {0, 6, 1}, color);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 255;
	Plano* planoIzquierda = new Plano({6, 0, 0}, {6, 0, 1}, {6, 1, 0}, color);

	color.rgbRed = 0;
	color.rgbGreen = 255;
	color.rgbBlue = 0;
	Plano* planoDerecha = new Plano({0, 0, 0}, {0, 1, 0}, {0, 0, 1}, color);

	color.rgbRed = 255;
	color.rgbGreen = 0;
	color.rgbBlue = 0;
	Plano* planoAdelante = new Plano({0, 0, -6}, {0, 1, -6}, {1, 0, -6}, color);

	elementos.push_back(planoAbajo);
	elementos.push_back(planoArriba);
	elementos.push_back(planoIzquierda);
	elementos.push_back(planoDerecha);
	elementos.push_back(planoAdelante);

	Esfera* esferaCentro = new Esfera(2, 3, 2, -3);

	elementos.push_back(esferaCentro);
}

Escena* Escena::instancia = nullptr;

Escena* Escena::getInstance() {
	if (instancia == nullptr) {
		instancia = new Escena();
	}
	return instancia;
}

Escena::~Escena() {}
