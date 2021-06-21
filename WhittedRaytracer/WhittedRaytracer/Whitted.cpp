#include "../SDL2-2.0.12/include/SDL.h"
#include "../Freeimage/FreeImage.h"

#include <stdio.h>
#include <conio.h>
#include <time.h>       /* time */
#include <direct.h>
#include <tchar.h>

#include "Plano.h"
#include "Escena.h"
#include "Pantalla.h"
#include "Rayo.h"
#include "Camara.h"

#include <vector>
#include <iostream>
#include <string>

#include "Whitted.h"
#include "Esfera.h"

const int PROFUNDIDAD_MAX = 5;

Whitted::Whitted() {};

Whitted::~Whitted() {};

float clamp(const float& lo, const float& hi, const float& v) {
    return std::max(lo, std::min(hi, v));
}

vec3 refractSCRATCHPIXEL(const vec3& I, const vec3& N, const float& ior)
{
    float cosi = clamp(-1, 1, dot(I, N));
    float etai = 1, etat = ior;
    vec3 n = N;
    if (cosi < 0) { cosi = -cosi; }
    else { std::swap(etai, etat); n = -N; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);

    if (k < 0) {
        return { 0, 0, 0 };
    }
    else {
        return eta * I + (eta * cosi - sqrtf(k)) * n;
    }
}

RGBQUAD Whitted::traza_RR(Rayo* rayo_RR, int profundidad) {
    RGBQUAD color = { 0, 0, 0 };
    Escena* escena = Escena::getInstance();

    int indiceMasCerca = -1;    //elemento donde voy a guardar el mas chico
    float distancia = 100000;

    vec3 interseccionMasCercana;

    for (int i = 0; i < escena->elementos.size(); i++) {
        float t = escena->elementos[i]->interseccionRayo(rayo_RR);

        if (t > 1e-3) {   // intersecta - PROBAR CON MAYOR Q CERO
            vec3 interseccion = rayo_RR->origen + rayo_RR->direccion * t;
            float distanciaNueva = distance(rayo_RR->origen, interseccion);

            if (distanciaNueva < distancia) {
                distancia = distanciaNueva;
                indiceMasCerca = i;
                interseccionMasCercana = interseccion;
            }
        }
    }

    if (indiceMasCerca != -1) { //hay objeto intersectado
        //calculamos normal de la interseccion.
        vec3 normalInterseccion = escena->elementos[indiceMasCerca]->normalDelPunto(interseccionMasCercana);

        color = sombra_RR(indiceMasCerca, rayo_RR, interseccionMasCercana, normalInterseccion, profundidad);
        //color = escena->elementos[indiceMasCerca]->color;
    }

    return color;
}

RGBQUAD Whitted::sombra_RR(int indiceObjetoIntersectado, Rayo* rayo, vec3 interseccion, vec3 normal, int profundidad) {
    Escena* escena = Escena::getInstance();
    Elemento* objeto = escena->elementos[indiceObjetoIntersectado];

    vec3 intensidadLuzAmbiente = { 0.8, 0.8, 0.8 }; // { 0, 0, 0 }

    RGBQUAD resultadoAmbiente = { 0, 0, 0 };
    RGBQUAD resultadoDifuso = { 0, 0, 0 };
    RGBQUAD resultadoEspecular = { 0, 0, 0 };
    RGBQUAD resultadoFinal = { 0, 0, 0 };

    float coeficenteAmbiente = objeto->getAmbiente();
    resultadoAmbiente.rgbRed = intensidadLuzAmbiente.r * coeficenteAmbiente * objeto->color.rgbRed;
    resultadoAmbiente.rgbGreen = intensidadLuzAmbiente.g * coeficenteAmbiente * objeto->color.rgbGreen;
    resultadoAmbiente.rgbBlue = intensidadLuzAmbiente.b * coeficenteAmbiente * objeto->color.rgbBlue;

    resultadoFinal = resultadoAmbiente; // Arrancamos con el valor de ambiente y le sumamos los otros factores luego.

    for (int i = 0; i < escena->luces.size(); i++) {
        Rayo* rayoSecundario = new Rayo(interseccion - normal * 0.005f, escena->luces[i]->posicion, rayo->refraccionObjetoActual);

        float distanciaObjetoLuz = distance(interseccion, escena->luces[i]->posicion);
        vec3 intesidadLuzActual = escena->luces[i]->intesidad;

        Plano* p = dynamic_cast<Plano*>(objeto);
        float factorNormalLuz = 0;

        if (p != nullptr) {
            factorNormalLuz = dot(-normal, rayoSecundario->direccion); // la normal de los planos esta al revez la muy hdp
        } else {
            factorNormalLuz = dot(normal, rayoSecundario->direccion);
        }

        if (factorNormalLuz > 0) {
            for (int j = 0; j < escena->elementos.size(); j++) {
                if (j != indiceObjetoIntersectado) {
                    float t = escena->elementos[j]->interseccionRayo(rayoSecundario);
                    vec3 interseccionSecundario = rayoSecundario->origen + rayoSecundario->direccion * t;

                    // Calcular cuánta luz es bloqueada por superficies opacas y transparentes
                    if (t > 0 && distance(interseccion, interseccionSecundario) < distanciaObjetoLuz) {
                        float coeficenteTransmision = escena->elementos[j]->getTransmision();
                        intesidadLuzActual.r = intesidadLuzActual.r * coeficenteTransmision * escena->elementos[j]->color.rgbRed / 255;
                        intesidadLuzActual.g = intesidadLuzActual.r * coeficenteTransmision * escena->elementos[j]->color.rgbGreen / 255;
                        intesidadLuzActual.b = intesidadLuzActual.r * coeficenteTransmision * escena->elementos[j]->color.rgbBlue / 255;
                    }
                }
            }

            // usarlo para escalar los términos difusos y especulares antes de añadirlos a color;

            float coeficenteDifuso = objeto->getDifusa();
            resultadoDifuso.rgbRed = clamp(0, 255, resultadoDifuso.rgbRed + factorNormalLuz * coeficenteDifuso * objeto->color.rgbRed);
            resultadoDifuso.rgbGreen = clamp(0, 255, resultadoDifuso.rgbGreen + factorNormalLuz * coeficenteDifuso * objeto->color.rgbGreen);
            resultadoDifuso.rgbBlue = clamp(0, 255, resultadoDifuso.rgbBlue + factorNormalLuz * coeficenteDifuso * objeto->color.rgbBlue);

            float RV = dot(reflect(-rayoSecundario->direccion, normal), -rayo->direccion);  // puede q sea con - en ambos rayos o ninguno jeje
            if (RV > 0) { // si hay reflexion
                float coeficenteEspecular = objeto->getEspecular();
                resultadoEspecular.rgbRed += pow(RV, 2) * coeficenteEspecular * objeto->color.rgbRed;
                resultadoEspecular.rgbGreen += pow(RV, 2) * coeficenteEspecular * objeto->color.rgbGreen;
                resultadoEspecular.rgbBlue += pow(RV, 2) * coeficenteEspecular * objeto->color.rgbBlue;
            }

            float factorAtenuacion = 1 / distanciaObjetoLuz;

            resultadoFinal.rgbRed = clamp(0, 255, resultadoFinal.rgbRed + ((resultadoDifuso.rgbRed + resultadoEspecular.rgbRed) * factorAtenuacion * intesidadLuzActual.r));
            resultadoFinal.rgbGreen = clamp(0, 255, resultadoFinal.rgbGreen + ((resultadoDifuso.rgbGreen + resultadoEspecular.rgbGreen) * factorAtenuacion * intesidadLuzActual.g));
            resultadoFinal.rgbBlue = clamp(0, 255, resultadoFinal.rgbBlue + ((resultadoDifuso.rgbBlue + resultadoEspecular.rgbBlue) * factorAtenuacion * intesidadLuzActual.b));
        }
    }

    if (profundidad < PROFUNDIDAD_MAX) {
        float coeficienteReflexion = objeto->getReflexion();
        if (coeficienteReflexion > 0) {

            vec3 auxNormal = normal;
            Esfera* e = dynamic_cast<Esfera*>(objeto);
            if (e != nullptr) {
                auxNormal = -normal;
            }

            Rayo* rayoReflejado = crearRayoConDireccion(interseccion - auxNormal * 0.005f, reflect(rayo->direccion, auxNormal), rayo->refraccionObjetoActual);
            RGBQUAD colorReflejado = traza_RR(rayoReflejado, profundidad + 1);

            resultadoFinal.rgbRed = clamp(0, 255, resultadoFinal.rgbRed + colorReflejado.rgbRed * objeto->getEspecular() * coeficienteReflexion);
            resultadoFinal.rgbGreen = clamp(0, 255, resultadoFinal.rgbGreen + colorReflejado.rgbGreen * objeto->getEspecular() * coeficienteReflexion);
            resultadoFinal.rgbBlue = clamp(0, 255, resultadoFinal.rgbBlue + colorReflejado.rgbBlue * objeto->getEspecular() * coeficienteReflexion);
        }

        float coeficienteTransmision = objeto->getTransmision();
        if (coeficienteTransmision > 0) {
            float indiceRefraccion = objeto->getRefraccion();

            float anguloIncidencia = (double)acos(dot(normal, -rayo->direccion)) * 180 / PI;     //HAY ALGO RARO EN LOS ANGULOS - ENTRA SIEMPRE
            float anguloCritico = (double)asin(indiceRefraccion / rayo->refraccionObjetoActual) * 180 / PI;

            if (!((rayo->refraccionObjetoActual >= indiceRefraccion) && (anguloIncidencia > anguloCritico))) {

                float refraccionProximoRayo = indiceRefraccion;
                if (dot(normal, rayo->direccion) > 0) {
                    refraccionProximoRayo = 1.00029;
                }

                Rayo* rayoRefractado = crearRayoConDireccion(interseccion + normal * 0.0001f, refract(rayo->direccion, normal, rayo->refraccionObjetoActual/refraccionProximoRayo), refraccionProximoRayo);
                RGBQUAD colorRefractado = traza_RR(rayoRefractado, profundidad + 1);

                resultadoFinal.rgbRed = clamp(0, 255, resultadoFinal.rgbRed + colorRefractado.rgbRed);
                resultadoFinal.rgbGreen = clamp(0, 255, resultadoFinal.rgbGreen + colorRefractado.rgbGreen);
                resultadoFinal.rgbBlue = clamp(0, 255, resultadoFinal.rgbBlue + colorRefractado.rgbBlue);
            }
        }
    }

    return resultadoFinal;
}

Rayo* Whitted::crearRayoConDireccion(vec3 origen, vec3 direccion, float refraccionObjetoActual) {
    Rayo* res = new Rayo(origen, { 0,0,0 }, refraccionObjetoActual);
    res->direccion = direccion;

    return res;
}
