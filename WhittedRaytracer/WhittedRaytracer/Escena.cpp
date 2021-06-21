#include "Escena.h"
#include "Esfera.h"
#include "Cilindro.h"
#include "Triangulo.h"

Escena::Escena() {

	// CREACION TEMPORAL DE OBJETOS
	RGBQUAD color;

	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Plano* planoAbajo = new Plano(
		{0, 0, 0}, {1, 0, 0}, { 0, 0, 1 },
		color, 0.5, 0.9, 0.3,
		0, 0, 0
	);

	color.rgbRed = 100;
	color.rgbGreen = 100;
	color.rgbBlue = 100;
	Plano* planoArriba = new Plano(
		{0, 6, 0}, {0, 6, 1}, { 1, 6, 0 },
		color, 0.7, 0.9, 0.3,
		0, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 255;
	Plano* planoIzquierda = new Plano(
		{6, 0, 0}, {6, 1, 0}, { 6, 0, 1 },
		color, 0.2, 0.9, 0.3,
		0, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 255;
	color.rgbBlue = 0;
	Plano* planoDerecha = new Plano(
		{0, 0, 0}, {0, 0, 1}, { 0, 1, 0 },
		color, 0.3, 0.9, 0.3,
		0, 0, 0
	);

	color.rgbRed = 255;
	color.rgbGreen = 0;
	color.rgbBlue = 0;
	Plano* planoAdelante = new Plano(
		{0, 0, -6}, { 0, 1, -6 }, { 1, 0, -6 },
		color, 0.6, 0.9, 0.3,
		0, 0, 0
	);

	color.rgbRed = 0;
	color.rgbGreen = 0;
	color.rgbBlue = 0;
	Plano* planoAtras = new Plano(
		{ 0, 0, 7 }, { 1, 0, 7 }, { 0, 1, 7 },
		color, 0.3, 0.9, 0.3,
		0, 0, 0
	);

	elementos.push_back(planoAbajo);
	elementos.push_back(planoArriba);
	elementos.push_back(planoIzquierda);
	elementos.push_back(planoDerecha);
	elementos.push_back(planoAdelante);
	elementos.push_back(planoAtras);

	// ESFERAS ----------------------------------------------------------------------
	color.rgbRed = 254;
	color.rgbGreen = 254;
	color.rgbBlue = 254;
	Esfera* fullReflectiva = new Esfera(
		0.3, 4, 1.3, -4, 
		color, 0.7, 0, 1,
		1, 0, 0
	);

	color.rgbRed = 254;
	color.rgbGreen = 254;
	color.rgbBlue = 254;
	Esfera* fullRR = new Esfera(
		0.8, 5, 4, -4,
		color, 0.1, 0.9, 0.4,
		1, 2.49, 1
	);

	elementos.push_back(fullReflectiva);
	elementos.push_back(fullRR);

	// CILINDROS -----------------------------------------------------------

	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Cilindro* cilindroVidrio = new Cilindro(
		{ 2, 1, -4 }, 0.4, 1,
		color, 0.4, 0.9, 0.9,
		0, 1.3, 1
	);

	elementos.push_back(cilindroVidrio);

	// CILINDROS -----------------------------------------------------------

	color.rgbRed = 119;
	color.rgbGreen = 37;
	color.rgbBlue = 207;

	Triangulo* base1 = new Triangulo(
		{ 1, 1, -3 }, { 1, 1, -5 }, { 5, 1, -3 },
		color, 0.3, 0.9, 0.9,
		0.3, 1.3, 0
	);

	Triangulo* base2 = new Triangulo(
		{ 1, 1, -5 }, { 5, 1, -5 }, { 5, 1, -3 },
		color, 0.3, 0.9, 0.9,
		0.3, 1.3, 0
	);

	Triangulo* ladoFrente = new Triangulo(
		{ 1, 1, -3 }, { 5, 1, -3 }, { 2, 0, -3 },
		color, 0.3, 0.9, 0.9,
		0, 1.3, 0
	);

	Triangulo* ladoIzq = new Triangulo(
		{ 1, 1, -3 }, { 2, 0, -3 }, { 1, 1, -5 },
		color, 0.3, 0.9, 0.9,
		0, 1.3, 0
	);

	Triangulo* ladoDer = new Triangulo(
		{ 5, 1, -3 }, { 5, 1, -5 }, { 2, 0, -3 },
		color, 0.3, 0.9, 0.9,
		0, 1.3, 0
	);

	Triangulo* ladoAtras = new Triangulo(
		{ 1, 1, -5 }, { 2, 0, -3 }, { 5, 1, -5 },
		color, 0.3, 0.9, 0.9,
		0, 1.3, 0
	);

	elementos.push_back(base1);
	elementos.push_back(base2);
	elementos.push_back(ladoFrente);
	elementos.push_back(ladoIzq);
	elementos.push_back(ladoDer);
	elementos.push_back(ladoAtras);

	// CREACION TEMPORAL DE LUCES ------------------------------------------
	color.rgbRed = 255;
	color.rgbGreen = 255;
	color.rgbBlue = 255;
	Luz* luzTecho = new Luz({3, 5, -4}, {0.8, 0.8, 0.8}, color);
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
