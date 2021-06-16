#pragma once

#include "Plano.h"
#include "Elemento.h"
#include "Luz.h"

#include <vector>

using namespace std;

class Escena {
	private:
		static Escena* instancia;

	public:
		Escena();
		~Escena();

		vector <Elemento*> elementos;
		vector <Luz*> luces;

		static Escena* getInstance();
};

