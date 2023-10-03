#pragma once

#include "../Freeimage/FreeImage.h"
#include "nanoflann/nanoflann.h"
#include "nanoflann/utils.h"
#include "Escena.h"
#include "Foton.h"

#include <vector>
#include <iostream>
#include <string>
#include <random> 
#include <functional> 
#include <list> 
#include <embree3/rtcore.h>

mt19937 engine;  //Mersenne twister MT19937

uniform_real_distribution<double> distribution1(0.0, 1.0);
static auto generator1 = std::bind(distribution1, engine);

uniform_real_distribution<double> distribution2(-1.0, 1.0);
static auto generator2 = std::bind(distribution2, engine);

class PhotonMapper
{
public:
    void generarMapaDeFotones(PointCloud& listaFotones, RTCScene &scene) {
        Escena* escena = Escena::getInstance();

        vec3 dirFoton = { 0, 0, 0 };

        // Calcular la potencia total para poder ver cuantos fotones aporta cada luz a la escena
        int potenciaTotal = 0;
        for (int i = 0; i < escena->luces.size(); i++) {
            potenciaTotal += escena->luces[i]->watts;
        }

        // Iterar sobre todas las luces de la escena emitiendo fotones
        for (int i = 0; i < escena->luces.size(); i++) {
            int fotonesAEmitir = escena->luces[i]->watts * escena->cantidadDeFotones / potenciaTotal;
            int fotonesEmitidos = 0;

            //cout << "Fotones emitidos: " << fotonesEmitidos << "\n\n";
            while (fotonesEmitidos < fotonesAEmitir) {
                do {
                    dirFoton.x = generator2();
                    dirFoton.y = generator2();
                    dirFoton.z = generator2();
                } while (pow(dirFoton.x, 2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);
                trazarFoton(escena->luces[i]->color, escena->luces[i]->posicion, dirFoton, listaFotones, 0, scene);
                fotonesEmitidos++;
                // QUESTION: SCALE POWER OF STORED PHOTONS ??

                /*if (fotonesEmitidos % 100 == 0) {
                    cout << "Fotones Emitidos: " << fotonesEmitidos << "\n";
                }*/
            }
        }
    }
    void trazarFoton(RGBQUAD color, vec3 origen, vec3 direccion, PointCloud& listaFotones, int profundidad, RTCScene &scene) {
        RTCRayHit rayhit;
        rayhit.ray.org_x = origen.x;
        rayhit.ray.org_y = origen.y;
        rayhit.ray.org_z = origen.z;
        rayhit.ray.dir_x = direccion.x;
        rayhit.ray.dir_y = direccion.y;
        rayhit.ray.dir_z = direccion.z;
        rayhit.ray.tnear = 0.f;
        rayhit.ray.tfar = std::numeric_limits<float>::infinity();
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        Escena* escena = Escena::getInstance();

        vec3 interseccionMasCercana;
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);
        rtcIntersect1(scene, &context, &rayhit);
        if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            vec3 rayHitOrg = { rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z };
            vec3 rayHitDir = { rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z };
            vec3 interseccionMasCercana = rayHitOrg + rayHitDir * rayhit.ray.tfar;
            vec3 reflexionEspecular = escena->elementos[rayhit.hit.geomID]->coeficienteReflexionEspecular;
            vec3 reflexionDifusa = escena->elementos[rayhit.hit.geomID]->coeficienteReflexionDifusa;
            vec3 normalInterseccion = { rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z };
            normalInterseccion = normalize(normalInterseccion);

            double a = generator1();

            double dividendoDifusa = max({ reflexionDifusa.r * color.rgbRed, reflexionDifusa.g * color.rgbGreen, reflexionDifusa.b * color.rgbBlue });
            double divisorDifusa = max({ color.rgbRed, color.rgbGreen, color.rgbBlue });
            double factorPotenciaDifusa = dividendoDifusa / divisorDifusa; // Pd

            double dividendoEspecular = max({ reflexionEspecular.r * color.rgbRed, reflexionEspecular.g * color.rgbGreen, reflexionEspecular.b * color.rgbBlue });
            double divisorEspecular = max({ color.rgbRed, color.rgbGreen, color.rgbBlue });
            double factorPotenciaEspecular = dividendoEspecular / divisorEspecular; // Ps

            vec3 direccionReflejada{};
            if (a < factorPotenciaDifusa) {
                RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.r / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.g / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.b / factorPotenciaDifusa };
                potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
                if (profundidad > 0) {
                    listaFotones.pts.push_back(Foton(interseccionMasCercana, color, 0, 0, 0)); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
                }

                do {
                    direccionReflejada = { generator2(), generator2(), generator2() };
                } while (dot(normalInterseccion, direccionReflejada) <= 0);
                trazarFoton(potenciaReflejada, interseccionMasCercana + 0.01f * normalInterseccion, direccionReflejada, listaFotones, profundidad + 1, scene);

            }
            else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
                RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.r / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.g / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.b / factorPotenciaEspecular };
                potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
                direccionReflejada = reflect(rayHitDir, normalInterseccion);
                direccionReflejada = interseccionMasCercana - direccionReflejada;
                trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, profundidad + 1, scene);

            }
            else {
                //if (elementoIntersectado->getDifusa() > 0.0) {
                listaFotones.pts.push_back(Foton(interseccionMasCercana, color, 0, 0, 0));
               // }
            }
        }
    }
};

