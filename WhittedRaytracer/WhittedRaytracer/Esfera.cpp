#include "Esfera.h"

Esfera::Esfera(
    float radio, float a, float b, float c,
    RGBQUAD color, float ambiente, float difusa, float especular, 
    float reflexion, float refraccion, float transmision,
    vec3 coeficienteReflexionDifusa,
    vec3 coeficienteReflexionEspecular) {

	this->radio = radio;
    this->posicion = { a, b, c };
    this->color = color;

    this->ambiente = ambiente;
    this->difusa = difusa;
    this->especular = especular;

    this->reflexion = reflexion;
    this->refraccion = refraccion;
    this->transmision = transmision;

    this->coeficienteReflexionDifusa = coeficienteReflexionDifusa;
    this->coeficienteReflexionEspecular = coeficienteReflexionEspecular;
}

Esfera::~Esfera() {}

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = -0.5 * b / a;
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }

    if (x0 > x1) {
        float temp = x0;
        x0 = x1;
        x1 = temp;
    }

    return true;
}

float Esfera::interseccionRayo(Rayo* rayo) {
    float t0, t1; // solutions for t if the ray intersects 

    float radio2 = this->radio * this->radio;
    
    vec3 L = rayo->origen - this->posicion;
    float a = dot(rayo->direccion, rayo->direccion);
    float b = 2 * dot(rayo->direccion, L);
    float c = dot(L, L) - radio2;
    if (!solveQuadratic(a, b, c, t0, t1)) return 0;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead 
        if (t0 < 0) return 0; // both t0 and t1 are negative 
    }

    return t0;
}

vec3 Esfera::normalDelPunto(vec3 punto) {
    return normalize(punto - this->posicion);
}
