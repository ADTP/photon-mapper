#include "Escena.h"
#include "Camara.h"
#include "Pantalla.h"

using namespace std;

Escena::Escena() {
	xml_document doc;
	doc.load_file("../escena.xml");
	xml_node escena = doc.child("escena");

	// ESCENA
	cantidadDeFotones = escena.attribute("cantidadDeFotones").as_int();
	generarMapas = escena.attribute("generarMapas").as_bool();

	// PANTALLA
	xml_node nodoPantalla = escena.child("pantalla");
	int ancho = nodoPantalla.attribute("ancho").as_int();
	int altura = nodoPantalla.attribute("altura").as_int();
	Pantalla::setInstance(new Pantalla(ancho, altura));

	// CAMARA
	xml_node nodoCamara = escena.child("camara");
	float fovHorizontal = nodoCamara.attribute("fovHorizontal").as_float();
	float distanciaMalla = nodoCamara.attribute("distanciaMalla").as_float();

	xml_node posicion = nodoCamara.child("posicion");
	vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };

	xml_node arriba = nodoCamara.child("arriba");
	vec3 up = { arriba.attribute("x").as_float(), arriba.attribute("y").as_float(), arriba.attribute("z").as_float() };

	xml_node adelante = nodoCamara.child("adelante");
	vec3 front = { adelante.attribute("x").as_float(), adelante.attribute("y").as_float(), adelante.attribute("z").as_float() };
	
	Camara::setInstance(new Camara(pos, up, front, fovHorizontal, distanciaMalla));

	// LUCES
	xml_node nodoLuces = escena.child("luces");
	xml_node_iterator iteradorLuz = nodoLuces.begin();
	while (iteradorLuz != nodoLuces.end()) {
		int watts = iteradorLuz->attribute("watts").as_int();

		xml_node color = iteradorLuz->child("color");
		RGBQUAD colorLuz;
		colorLuz.rgbRed = color.attribute("r").as_float();
		colorLuz.rgbGreen = color.attribute("g").as_float();
		colorLuz.rgbBlue = color.attribute("b").as_float();

		xml_node posicion = iteradorLuz->child("posicion");
		vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };

		Luz* luzTecho = new Luz(pos, {}, colorLuz, watts);
		luces.push_back(luzTecho);
		iteradorLuz++;
	}

	// ELEMENTOS
	xml_node nodoElementos = escena.child("elementos");
	xml_node_iterator iteradorElementos = nodoElementos.begin();
	while (iteradorElementos != nodoElementos.end()) {
		string nombreArchivo = iteradorElementos->attribute("archivo").as_string();
		float indiceRefraccion = iteradorElementos->attribute("indiceRefraccion").as_float();
		float radio = iteradorElementos->attribute("radio").as_float();

		xml_node color = iteradorElementos->child("color");
		RGBQUAD colorElemento;
		colorElemento.rgbRed = color.attribute("r").as_float();
		colorElemento.rgbGreen = color.attribute("g").as_float();
		colorElemento.rgbBlue = color.attribute("b").as_float();

		xml_node posicion = iteradorElementos->child("posicion");
		vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };

		xml_node coefDifusa = iteradorElementos->child("coeficienteReflexionDifusa");
		vec3 coeficienteReflexionDifusa = { coefDifusa.attribute("r").as_float(), coefDifusa.attribute("g").as_float(), coefDifusa.attribute("b").as_float() };

		xml_node coefEspecular = iteradorElementos->child("coeficienteReflexionEspecular");
		vec3 coeficienteReflexionEspecular = { coefEspecular.attribute("r").as_float(), coefEspecular.attribute("g").as_float(), coefEspecular.attribute("b").as_float() };

		Elemento* elemento = new Elemento(nombreArchivo, colorElemento, pos, coeficienteReflexionDifusa, coeficienteReflexionEspecular, indiceRefraccion, radio);
		elementos.push_back(elemento);
		
		iteradorElementos++;
	}
}

Escena* Escena::instancia = nullptr;

Escena* Escena::getInstance() {
	if (instancia == nullptr) {
		instancia = new Escena();
	}
	return instancia;
}

Escena::~Escena() {}
