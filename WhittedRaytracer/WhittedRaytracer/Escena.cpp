#include "Escena.h"
#include "Esfera.h"
#include "Cilindro.h"
#include "Triangulo.h"

Escena::Escena() {

	// CREACION TEMPORAL DE OBJETOS
	RGBQUAD color;



	xml_document doc;
	doc.load_file("../escena.xml");
	xml_node elems = doc.child("elementos");
	//*******************************************************PLANOS********************************************
	//this->parsearPlanos(elems);
	xml_node planos = elems.child("planos");
	xml_node_iterator it = planos.begin();
	while (it != planos.end()) {
		xml_node puntos = it->first_child();
		xml_node color = it->first_child().next_sibling();
		xml_node_iterator it2 = puntos.begin();
		vec3 p1 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p2 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p3 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		RGBQUAD colorGuardar;
		colorGuardar.rgbRed = color.attribute("r").as_float();
		colorGuardar.rgbGreen = color.attribute("g").as_float();
		colorGuardar.rgbBlue = color.attribute("b").as_float();
		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();
		Plano* plano = new Plano(p1, p2, p3, colorGuardar, ambiente, difusa, especular, reflexion, refraccion, transmision);
		elementos.push_back(plano);
		it++;
	}
	//*******************************************************ESFERA********************************************
	xml_node esferas = elems.child("esferas");
	it = esferas.begin();
	while (it != esferas.end()) {
		xml_node color = it->first_child();
		RGBQUAD colorEsfera;
		colorEsfera.rgbRed = color.attribute("r").as_float();
		colorEsfera.rgbGreen = color.attribute("g").as_float();
		colorEsfera.rgbBlue = color.attribute("b").as_float();
		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();
		Esfera* esferaNueva = new Esfera(it->attribute("radio").as_float(), it->attribute("a").as_float(), it->attribute("b").as_float(), it->attribute("c").as_float(),
			colorEsfera, ambiente, difusa, especular, reflexion, refraccion, transmision);
		elementos.push_back(esferaNueva);
		it++;
	}
	//*******************************************************LUCES********************************************
	xml_node lucesXML = elems.child("luces");
	it = lucesXML.begin();
	while (it != lucesXML.end()) {
		xml_node color = it->first_child();
		xml_node posicion = it->first_child().next_sibling();
		xml_node intensidad = it->first_child().next_sibling().next_sibling();
		vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };
		vec3 intense = { intensidad.attribute("i1").as_float(), intensidad.attribute("i2").as_float(), intensidad.attribute("i3").as_float() };
		RGBQUAD colorLuz;
		colorLuz.rgbRed = color.attribute("r").as_float();
		colorLuz.rgbGreen = color.attribute("g").as_float();
		colorLuz.rgbBlue = color.attribute("b").as_float();
		Luz* luzTecho = new Luz(pos, intense, colorLuz);
		luces.push_back(luzTecho);
		it++;
	}
	//*******************************************************CAMARA********************************************
	xml_node camaraXML = elems.child("camara");
	xml_node posicion = camaraXML.first_child();
	xml_node arriba = camaraXML.first_child().next_sibling();
	xml_node adelante = camaraXML.first_child().next_sibling().next_sibling();
	vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };
	vec3 up = { arriba.attribute("x").as_float(), arriba.attribute("y").as_float(), arriba.attribute("z").as_float() };
	vec3 front = { adelante.attribute("x").as_float(), adelante.attribute("y").as_float(), adelante.attribute("z").as_float() };
	float fovV = camaraXML.attribute("fovVertical").as_float();
	float fovH = camaraXML.attribute("fovHorizontal").as_float();
	float distanciaMalla = camaraXML.attribute("distanciaMalla").as_float();
	//Camara* camara = new Camara(pos, up, front, fovV, fovH, distanciaMalla);

	//*******************************************************CILINDROS********************************************
	xml_node cilindrosXML = elems.child("cilindros");
	it = cilindrosXML.begin();
	while (it != cilindrosXML.end()) {
		xml_node color = it->first_child();
		xml_node posicion = it->first_child().next_sibling();
		RGBQUAD colorCilindro;
		colorCilindro.rgbRed = color.attribute("r").as_float();
		colorCilindro.rgbGreen = color.attribute("g").as_float();
		colorCilindro.rgbBlue = color.attribute("b").as_float();
		vec3 pos = { posicion.attribute("x").as_float(),posicion.attribute("y").as_float(),posicion.attribute("z").as_float() };

		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();

		float radio = it->attribute("radio").as_float();
		float altura = it->attribute("altura").as_float();
		Cilindro* cilindroNuevo = new Cilindro(pos,radio, altura,
			colorCilindro, ambiente, difusa, especular, reflexion, refraccion, transmision);
		elementos.push_back(cilindroNuevo);
		it++;
	}

	//*******************************************************TRIANGULOS********************************************
	//this->parsearPlanos(elems);
	xml_node triangulosXML = elems.child("triangulos");
	it = triangulosXML.begin();
	while (it != triangulosXML.end()) {
		xml_node puntos = it->first_child();
		xml_node color = it->first_child().next_sibling();
		xml_node_iterator it2 = puntos.begin();
		vec3 p1 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p2 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p3 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		RGBQUAD colorGuardar;
		colorGuardar.rgbRed = color.attribute("r").as_float();
		colorGuardar.rgbGreen = color.attribute("g").as_float();
		colorGuardar.rgbBlue = color.attribute("b").as_float();
		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();
		Triangulo* trianguloNuevo = new Triangulo(p1, p2, p3, colorGuardar, ambiente, difusa, especular, reflexion, refraccion, transmision);
		elementos.push_back(trianguloNuevo);
		it++;
	}



	//color.rgbRed = 255;
	//color.rgbGreen = 255;
	//color.rgbBlue = 255;
	//Plano* planoAbajo = new Plano(
	//	{0, 0, 0}, {1, 0, 0}, { 0, 0, 1 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//color.rgbRed = 100;
	//color.rgbGreen = 100;
	//color.rgbBlue = 100;
	//Plano* planoArriba = new Plano(
	//	{0, 6, 0}, {0, 6, 1}, { 1, 6, 0 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//color.rgbRed = 0;
	//color.rgbGreen = 0;
	//color.rgbBlue = 255;
	//Plano* planoIzquierda = new Plano(
	//	{6, 0, 0}, {6, 1, 0}, { 6, 0, 1 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//color.rgbRed = 0;
	//color.rgbGreen = 255;
	//color.rgbBlue = 0;
	//Plano* planoDerecha = new Plano(
	//	{0, 0, 0}, {0, 0, 1}, { 0, 1, 0 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//color.rgbRed = 255;
	//color.rgbGreen = 0;
	//color.rgbBlue = 0;
	//Plano* planoAdelante = new Plano(
	//	{0, 0, -6}, { 0, 1, -6 }, { 1, 0, -6 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//color.rgbRed = 0;
	//color.rgbGreen = 0;
	//color.rgbBlue = 0;
	//Plano* planoAtras = new Plano(
	//	{ 0, 0, 7 }, { 1, 0, 7 }, { 0, 1, 7 },
	//	color, 0.3, 0.9, 0.3,
	//	0, 0, 0
	//);

	//elementos.push_back(planoAbajo);
	//elementos.push_back(planoArriba);
	//elementos.push_back(planoIzquierda);
	//elementos.push_back(planoDerecha);
	//elementos.push_back(planoAdelante);
	//elementos.push_back(planoAtras);

	//// ESFERAS ----------------------------------------------------------------------
	//color.rgbRed = 254;
	//color.rgbGreen = 254;
	//color.rgbBlue = 254;
	//Esfera* fullReflectiva = new Esfera(
	//	0.3, 4, 1.3, -4, 
	//	color, 0.3, 0, 1,
	//	1, 0, 0
	//);

	//color.rgbRed = 254;
	//color.rgbGreen = 254;
	//color.rgbBlue = 254;
	//Esfera* fullRR = new Esfera(
	//	0.8, 5, 4, -4,
	//	color, 0.3, 0.9, 0.4,
	//	1, 2.49, 1
	//);

	//elementos.push_back(fullReflectiva);
	//elementos.push_back(fullRR);

	//// CILINDROS -----------------------------------------------------------

	//color.rgbRed = 255;
	//color.rgbGreen = 255;
	//color.rgbBlue = 255;
	//Cilindro* cilindroVidrio = new Cilindro(
	//	{ 2, 1, -4 }, 0.4, 1,
	//	color, 0.3, 0.9, 0.9,
	//	0, 1.3, 1
	//);

	//elementos.push_back(cilindroVidrio);

	//// CILINDROS -----------------------------------------------------------

	//color.rgbRed = 119;
	//color.rgbGreen = 37;
	//color.rgbBlue = 207;

	//Triangulo* base1 = new Triangulo(
	//	{ 1, 1, -3 }, { 1, 1, -5 }, { 5, 1, -3 },
	//	color, 0.3, 0.9, 0.9,
	//	0.3, 1.3, 0
	//);

	//Triangulo* base2 = new Triangulo(
	//	{ 1, 1, -5 }, { 5, 1, -5 }, { 5, 1, -3 },
	//	color, 0.3, 0.9, 0.9,
	//	0.3, 1.3, 0
	//);

	//Triangulo* ladoFrente = new Triangulo(
	//	{ 1, 1, -3 }, { 5, 1, -3 }, { 2, 0, -3 },
	//	color, 0.3, 0.9, 0.9,
	//	0, 1.3, 0
	//);

	//Triangulo* ladoIzq = new Triangulo(
	//	{ 1, 1, -3 }, { 2, 0, -3 }, { 1, 1, -5 },
	//	color, 0.3, 0.9, 0.9,
	//	0, 1.3, 0
	//);

	//Triangulo* ladoDer = new Triangulo(
	//	{ 5, 1, -3 }, { 5, 1, -5 }, { 2, 0, -3 },
	//	color, 0.3, 0.9, 0.9,
	//	0, 1.3, 0
	//);

	//Triangulo* ladoAtras = new Triangulo(
	//	{ 1, 1, -5 }, { 2, 0, -3 }, { 5, 1, -5 },
	//	color, 0.3, 0.9, 0.9,
	//	0, 1.3, 0
	//);

	//elementos.push_back(base1);
	//elementos.push_back(base2);
	//elementos.push_back(ladoFrente);
	//elementos.push_back(ladoIzq);
	//elementos.push_back(ladoDer);
	//elementos.push_back(ladoAtras);

	//// CREACION TEMPORAL DE LUCES ------------------------------------------
	//color.rgbRed = 255;
	//color.rgbGreen = 255;
	//color.rgbBlue = 255;
	//Luz* luzTecho = new Luz({3, 5, -4}, {0.8, 0.8, 0.8}, color);
	//luces.push_back(luzTecho);

	///*Esfera* esferaLuz = new Esfera(0.05, 2, 3, -1, color, 0.9, 0.9, 0.9);
	//elementos.push_back(esferaLuz);*/

}

Escena* Escena::instancia = nullptr;

Escena* Escena::getInstance() {
	if (instancia == nullptr) {
		instancia = new Escena();
	}
	return instancia;
}

Escena::~Escena() {}
