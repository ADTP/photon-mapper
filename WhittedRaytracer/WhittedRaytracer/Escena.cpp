#include "Escena.h"

Escena::Escena() {
	Plano* planoAbajo = new Plano({0,0,0}, {1,0,0}, {0,0,1}, 0,0,0);
	Plano* planoArriba = new Plano({0,400,0}, { 1,400,0 }, { 0,400,1 }, 100, 100, 100);
	Plano* planoIzquierda = new Plano({0,0,0}, {0,1,0}, {0,0,1}, 255, 0, 0);
	Plano* planoDerecha = new Plano({600,0,0}, {600,1,0}, {600,0,1}, 0, 255, 0);
	Plano* planoAdelante = new Plano({0,0,500}, {0,1,500}, {1,0,500}, 0, 0, 255);
	planos[0] = planoAbajo;
	planos[1] = planoArriba;
	planos[2] = planoIzquierda;
	planos[3] = planoDerecha;
	planos[4] = planoAdelante;
}

Escena::~Escena() {}