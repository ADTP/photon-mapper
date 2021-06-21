#include "Cilindro.h"
#include "Plano.h"

Cilindro::Cilindro(
    vec3 posicion, float radio, float altura,
	RGBQUAD color, float ambiente, float difusa, float especular,
	float reflexion, float refraccion, float transmision) {

    this->posicion = posicion;

    this->radio = radio;
    this->altura = altura;
    
    this->color = color;
    this->ambiente = ambiente;
    this->difusa = difusa;
    this->especular = especular;

    this->reflexion = reflexion;
    this->refraccion = refraccion;
    this->transmision = transmision;
}

Cilindro::~Cilindro() {}

float Cilindro::interseccionRayo(Rayo* rayo) {
    float a = pow(rayo->direccion.x, 2) + pow(rayo->direccion.z, 2);
    float b = 2 * (rayo->direccion.x * (rayo->origen.x - this->posicion.x) + rayo->direccion.z * (rayo->origen.z - this->posicion.z));
    float c = pow((rayo->origen.x - this->posicion.x), 2) + pow((rayo->origen.z - this->posicion.z), 2) - pow(this->radio, 2);

    float delta = b * b - 4 * (a * c);
    if (fabs(delta) < 0.001) return -1.0;
    if (delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float t;

    if (t1 > t2) t = t2;
    else t = t1;

    float r = rayo->origen.y + t * rayo->direccion.y;

    if (!((r >= this->posicion.y) && (r <= this->posicion.y + this->altura))) {
        t = 0;
    }

    Plano* caraAbajo = new Plano(
        this->posicion, this->posicion + vec3{ 0, 0, 1 }, this->posicion + vec3{ 1, 0, 0 }, 
        {0,0,0}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    );

    float tAbajo = caraAbajo->interseccionRayo(rayo);

    Plano* caraArriba = new Plano(
        this->posicion + vec3{0, this->altura, 0}, this->posicion + vec3{ 1, this->altura, 0 }, this->posicion + vec3{ 0, this->altura, 1 },
        { 0,0,0 }, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    );

    float tArriba = caraArriba->interseccionRayo(rayo);

    vec3 rAbajo = rayo->origen + tAbajo * rayo->direccion;
    vec3 rArriba = rayo->origen + tArriba * rayo->direccion;

    float tAux = t;
    //if (pow(this->posicion.x - rAbajo.x, 2) + pow(this->posicion.z - rAbajo.z, 2) <= pow(radio, 2) || pow(this->posicion.x - rArriba.x, 2) + pow(this->posicion.z - rArriba.z, 2) <= pow(radio, 2)) {
    if (distance(this->posicion, rAbajo) <= radio || distance(this->posicion + vec3{0, altura, 0}, rArriba) <= radio) {
        if ((tArriba > tAbajo || tArriba == 0) && tAbajo > 0) {
            tAux = tAbajo; // NUNCA ENTRA ACA WTF
        }
        else if (tArriba > 0) {
            tAux = tArriba;
        }

        if (t == 0) {
            t = tAux;
        }
    }

    delete(caraAbajo);
    delete(caraArriba);

    return t;
}

vec3 Cilindro::normalDelPunto(vec3 punto) {
    if (distance(this->posicion, punto) <= radio || distance(this->posicion + vec3{ 0, altura, 0 }, punto) <= radio) {
        if (punto.y == (this->posicion.y + altura)) return vec3{ 0, 1, 0 };
        else return vec3{ 0, -1, 0 };
    }
    else {
        return normalize(vec3{ punto.x - this->posicion.x, 0, punto.z - this->posicion.z });
    }
}