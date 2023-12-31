#pragma once

#include "Elemento.h"
#include "Luz.h"

#include "pugixml.hpp"

#include <vector>

using namespace pugi;
using namespace std;

class Escena {
	private:
		static Escena* instancia;

	public:
		Escena();
		~Escena();

		vector <Elemento*> elementos;
		vector <Luz*> luces;
		
		int cantidadFotonesGlobal;
		int cantidadFotonesCausticas;
		
		bool generarMapas;
		bool heuristicaSombra;

		static Escena* getInstance();
};

