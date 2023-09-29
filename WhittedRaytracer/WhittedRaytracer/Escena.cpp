#include "Escena.h"
#include "Esfera.h"
#include "Cilindro.h"
#include "Triangulo.h"
#include "Camara.h"
#include "Pantalla.h"

using namespace std;

Escena::Escena() {

	RGBQUAD color;

	xml_document doc;
	doc.load_file("../escena.xml");
	xml_node elems = doc.child("elementos");

	//*******************************************************PLANOS********************************************
	xml_node planos = elems.child("planos");
	xml_node_iterator it = planos.begin();
	while (it != planos.end()) {
		xml_node puntos = it->first_child();
		xml_node_iterator it2 = puntos.begin();
		vec3 p1 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p2 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };
		it2++;
		vec3 p3 = { it2->attribute("x").as_float(),it2->attribute("y").as_float(),it2->attribute("z").as_float() };

		xml_node color = it->first_child().next_sibling();
		RGBQUAD colorGuardar;
		colorGuardar.rgbRed = color.attribute("r").as_float();
		colorGuardar.rgbGreen = color.attribute("g").as_float();
		colorGuardar.rgbBlue = color.attribute("b").as_float();

		xml_node iterCoeficienteDifusa = it->first_child().next_sibling().next_sibling();
		vec3 coeficienteReflexionDifusa;
		coeficienteReflexionDifusa.r = iterCoeficienteDifusa.attribute("r").as_float();
		coeficienteReflexionDifusa.g = iterCoeficienteDifusa.attribute("g").as_float();
		coeficienteReflexionDifusa.b = iterCoeficienteDifusa.attribute("b").as_float();

		xml_node iterCoeficienteEspecular = it->first_child().next_sibling().next_sibling().next_sibling();
		vec3 coeficienteReflexionEspecular;
		coeficienteReflexionEspecular.r = iterCoeficienteEspecular.attribute("r").as_float();
		coeficienteReflexionEspecular.g = iterCoeficienteEspecular.attribute("g").as_float();
		coeficienteReflexionEspecular.b = iterCoeficienteEspecular.attribute("b").as_float();

		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();

		Plano* plano = new Plano(p1, p2, p3, colorGuardar, ambiente, difusa, especular, reflexion, refraccion, transmision, coeficienteReflexionDifusa, coeficienteReflexionEspecular);
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

		xml_node iterCoeficienteDifusa = it->first_child().next_sibling();
		vec3 coeficienteReflexionDifusa;
		coeficienteReflexionDifusa.r = iterCoeficienteDifusa.attribute("r").as_float();
		coeficienteReflexionDifusa.g = iterCoeficienteDifusa.attribute("g").as_float();
		coeficienteReflexionDifusa.b = iterCoeficienteDifusa.attribute("b").as_float();

		xml_node iterCoeficienteEspecular = it->first_child().next_sibling().next_sibling();
		vec3 coeficienteReflexionEspecular;
		coeficienteReflexionEspecular.r = iterCoeficienteEspecular.attribute("r").as_float();
		coeficienteReflexionEspecular.g = iterCoeficienteEspecular.attribute("g").as_float();
		coeficienteReflexionEspecular.b = iterCoeficienteEspecular.attribute("b").as_float();

		float ambiente = it->attribute("ambiente").as_float();
		float difusa = it->attribute("difusa").as_float();
		float especular = it->attribute("especular").as_float();
		float reflexion = it->attribute("reflexion").as_float();
		float refraccion = it->attribute("refraccion").as_float();
		float transmision = it->attribute("transmision").as_float();

		Esfera* esferaNueva = new Esfera(
			it->attribute("radio").as_float(), it->attribute("a").as_float(), it->attribute("b").as_float(), it->attribute("c").as_float(),
			colorEsfera, ambiente, difusa, especular, 
			reflexion, refraccion, transmision,
			coeficienteReflexionDifusa,
			coeficienteReflexionEspecular
		);

		elementos.push_back(esferaNueva);
		it++;
	}

	//*******************************************************LUCES********************************************
	xml_node lucesXML = elems.child("luces");
	it = lucesXML.begin();
	while (it != lucesXML.end()) {
		xml_node color = it->first_child();
		RGBQUAD colorLuz;
		colorLuz.rgbRed = color.attribute("r").as_float();
		colorLuz.rgbGreen = color.attribute("g").as_float();
		colorLuz.rgbBlue = color.attribute("b").as_float();

		xml_node posicion = it->first_child().next_sibling();
		vec3 pos = { posicion.attribute("x").as_float(), posicion.attribute("y").as_float(), posicion.attribute("z").as_float() };
		
		xml_node intensidad = it->first_child().next_sibling().next_sibling();
		vec3 intense = { intensidad.attribute("i1").as_float(), intensidad.attribute("i2").as_float(), intensidad.attribute("i3").as_float() };
		
		Luz* luzTecho = new Luz(pos, intense, colorLuz, it->attribute("watts").as_int());
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
	float fovH = camaraXML.attribute("fovHorizontal").as_float();
	float distanciaMalla = camaraXML.attribute("distanciaMalla").as_float();
	
	Camara::setInstance(new Camara(pos, up, front, fovH, distanciaMalla));

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

		xml_node iterCoeficienteDifusa = it->first_child().next_sibling().next_sibling();
		vec3 coeficienteReflexionDifusa;
		coeficienteReflexionDifusa.r = iterCoeficienteDifusa.attribute("r").as_float();
		coeficienteReflexionDifusa.g = iterCoeficienteDifusa.attribute("g").as_float();
		coeficienteReflexionDifusa.b = iterCoeficienteDifusa.attribute("b").as_float();

		xml_node iterCoeficienteEspecular = it->first_child().next_sibling().next_sibling().next_sibling();
		vec3 coeficienteReflexionEspecular;
		coeficienteReflexionEspecular.r = iterCoeficienteEspecular.attribute("r").as_float();
		coeficienteReflexionEspecular.g = iterCoeficienteEspecular.attribute("g").as_float();
		coeficienteReflexionEspecular.b = iterCoeficienteEspecular.attribute("b").as_float();
		
		Triangulo* trianguloNuevo = new Triangulo(p1, p2, p3, colorGuardar, ambiente, difusa, especular, reflexion, refraccion, transmision, coeficienteReflexionDifusa, coeficienteReflexionEspecular);
		elementos.push_back(trianguloNuevo);
		it++;
	}

	//*******************************************************PANTALLA********************************************
	xml_node pantallaXML = elems.child("pantalla");
	int ancho = pantallaXML.attribute("ancho").as_int();
	int altura = pantallaXML.attribute("altura").as_int();
	Pantalla::setInstance(new Pantalla(ancho, altura));

	//*******************************************************ESCENA********************************************
	cantidadDeFotones = pantallaXML.attribute("cantidadDeFotones").as_int();
	generarMapas = pantallaXML.attribute("generarMapas").as_bool();
}

Escena* Escena::instancia = nullptr;

Escena* Escena::getInstance() {
	if (instancia == nullptr) {
		instancia = new Escena();
	}
	return instancia;
}

Escena::~Escena() {}
