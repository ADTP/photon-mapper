#include "Escena.h"
#include "Esfera.h"

Escena::Escena() {

	// CREACION TEMPORAL DE OBJETOS
	RGBQUAD color;

	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Plano* planoAbajo = new Plano(
		{0, 0, 0}, {1, 0, 0}, { 0, 0, 1 },
		color, 0.3, 0.9, 0.4,
		0.5, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 100;
	Plano* planoArriba = new Plano(
		{0, 6, 0}, {0, 6, 1}, { 1, 6, 0 },
		color, 0.3, 0.9, 0.4,
		0.5, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 255;
	Plano* planoIzquierda = new Plano(
		{6, 0, 0}, {6, 1, 0}, { 6, 0, 1 },
		color, 0.3, 0.9, 0.4,
		0.5, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 255;
	color.rgbBlue = 0;
	Plano* planoDerecha = new Plano(
		{0, 0, 0}, {0, 0, 1}, { 0, 1, 0 },
		color, 0.3, 0.9, 0.4,
		0.5, 0, 0
	);

	color.rgbRed = 255;
	color.rgbGreen = 0;
	color.rgbBlue = 0;
	Plano* planoAdelante = new Plano(
		{0, 0, -6}, { 0, 1, -6 }, { 1, 0, -6 },
		color, 0.3, 0.9, 0.4,
		0.5, 0, 0
	);

	//color.rgbRed = 100;
	//color.rgbGreen = 100;
	//color.rgbBlue = 100;
	//Plano* planoAtras = new Plano({ 0, 0, 5 }, { 0, 1, 5 }, { 1, 0, 5 }, color);


	elementos.push_back(planoAbajo);
	elementos.push_back(planoArriba);
	elementos.push_back(planoIzquierda);
	elementos.push_back(planoDerecha);
	elementos.push_back(planoAdelante);
	//elementos.push_back(planoAtras);

	// ESFERAS ----------------------------------------------------------------------
	color.rgbRed = 241;
	color.rgbGreen = 123;
	color.rgbBlue = 123;
	Esfera* ruby = new Esfera(
		0.5, 2, 3, -3, 
		color, 0.3, 0.9, 0.9,
		0, 1.77, 0.4
	);

	elementos.push_back(ruby);

	color.rgbRed = 233;
	color.rgbGreen = 233;
	color.rgbBlue = 255;
	Esfera* diamante = new Esfera(
		0.3, 4, 4, -1,
		color, 0.3, 0.9, 0.4,
		0, 2.417, 0.7
	);

	elementos.push_back(diamante);

	color.rgbRed = 122;
	color.rgbGreen = 122;
	color.rgbBlue = 255;
	Esfera* agua = new Esfera(
		0.3, 4, 2, -1,
		color, 0.3, 0.9, 0.4,
		0, 1.333, 0.2
	);

	elementos.push_back(agua);

	// CREACION TEMPORAL DE LUCES
	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Luz* luzTecho = new Luz({2, 3, 1}, {1, 1, 1}, color);
	luces.push_back(luzTecho);

	/*Esfera* esferaLuz = new Esfera(0.05, 2, 3, -1, color, 0.9, 0.9, 0.9);
	elementos.push_back(esferaLuz);*/

}

Escena* Escena::instancia = nullptr;

Escena* Escena::getInstance() {
	if (instancia == nullptr) {
		instancia = new Escena();
	}
	return instancia;
}

Escena::~Escena() {}
