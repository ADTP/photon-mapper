#pragma once
#include <embree4/rtcore.h>
class Whitted
{
    public:
        Whitted();
        ~Whitted();

        RGBQUAD traza_RR(Rayo* rayo_RR, int profundidad);
        RGBQUAD sombra_RR(int indiceObjetoIntersectado, Rayo* rayo, vec3 interseccion, vec3 normal, int profundidad);

        Rayo* crearRayoConDireccion(vec3 origen, vec3 direccion, float refraccionObjetoActual);
};

