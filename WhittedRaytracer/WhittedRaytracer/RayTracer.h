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

RGBQUAD negro = { 0, 0, 0 };

using CustomKDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3>;

class RayTracer {
    public:
    
    RGBQUAD iluminacionDirecta(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;

            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
            normalRayoIncidente = normalize(normalRayoIncidente);
        
            RGBQUAD resultadoFinal = negro;

            for (Luz* luz : escena->luces) {

                RTCRayHit rayoSombra = trazarRayo(scene, interseccionRayoIncidente + normalRayoIncidente * 0.1f, luz->posicion);
                vec3 origenRayoSombra = { rayoSombra.ray.org_x, rayoSombra.ray.org_y, rayoSombra.ray.org_z };
                vec3 direccionRayoSombra = { rayoSombra.ray.dir_x , rayoSombra.ray.dir_y , rayoSombra.ray.dir_z };

                if (dot(normalRayoIncidente, direccionRayoSombra) > 0) { // Si la luz incide directo sobre el punto de interseccion
                    float distanciaLuz = length(luz->posicion - interseccionRayoIncidente);
                    float distanciaInterseccion = length((origenRayoSombra + direccionRayoSombra * rayoSombra.ray.tfar) - origenRayoSombra);

                    if (distanciaInterseccion > distanciaLuz) { // Si no hay objetos intermedios
                        resultadoFinal.rgbRed = std::min(255, (int)(resultadoFinal.rgbRed + luz->color.rgbRed * (1 / distanciaLuz) * luz->watts / 20));
                        resultadoFinal.rgbGreen = std::min(255, (int)(resultadoFinal.rgbGreen + luz->color.rgbGreen * (1 / distanciaLuz) * luz->watts / 20));
                        resultadoFinal.rgbBlue = std::min(255, (int)(resultadoFinal.rgbBlue + luz->color.rgbBlue * (1 / distanciaLuz) * luz->watts / 20));
                    
                    }
                }
            }

            Elemento* elementoIntersecado = escena->elementos[rayoIncidente.hit.geomID];

            resultadoFinal.rgbRed = std::min((int)resultadoFinal.rgbRed, 255) * elementoIntersecado->color.rgbRed / 255;
            resultadoFinal.rgbGreen = std::min((int)resultadoFinal.rgbGreen, 255) * elementoIntersecado->color.rgbGreen / 255;
            resultadoFinal.rgbBlue = std::min((int)resultadoFinal.rgbBlue, 255) * elementoIntersecado->color.rgbBlue / 255;

            return resultadoFinal;
        }

        return negro;
    };

    RGBQUAD iluminacionIndirecta(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena, PointCloud listaFotones, CustomKDTree* index) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };


            const float consulta[3] = { interseccionRayoIncidente.x, interseccionRayoIncidente.y, interseccionRayoIncidente.z };
            const float radioEsfera = 100000; // VER SI HACERLO CUSTOMIZABLE
            std::vector <nanoflann::ResultItem<uint32_t, float>> fotonesResultantes;
            
            nanoflann::SearchParameters params;
            params.sorted = true;

            index->radiusSearch(&consulta[0], radioEsfera, fotonesResultantes, params); // Devuelve en fotonesResultantes un vector de pares con el siguiente formato: (indice, distancia
            int cantFotonesResultantes = fotonesResultantes.size();

            if (cantFotonesResultantes > 0) {
                vec3 flujoAcumulado = { 0,0,0 };
                for (int i = 0; i < cantFotonesResultantes; i++) {
                    Foton foton = listaFotones.pts[fotonesResultantes[i].first];

                    float productoPunto = dot(foton.direccionIncidente, -normalRayoIncidente);

                    if (productoPunto > 0) {
                        flujoAcumulado.r += productoPunto * foton.potencia.rgbRed;
                        flujoAcumulado.g += productoPunto * foton.potencia.rgbGreen;
                        flujoAcumulado.b += productoPunto * foton.potencia.rgbBlue;
                    }
                }

                float distanciaFotonMasLejano = fotonesResultantes[cantFotonesResultantes - 1].second;
                float pi = 3.14159265358979323846;

                RGBQUAD resultado;
                resultado.rgbRed = flujoAcumulado.r / (pi * pow(distanciaFotonMasLejano, 2));
                resultado.rgbGreen = flujoAcumulado.r / (pi * pow(distanciaFotonMasLejano, 2));
                resultado.rgbBlue = flujoAcumulado.r / (pi * pow(distanciaFotonMasLejano, 2));

                return resultado;
            }
        }

        return negro;
    }

    RTCRayHit trazarRayo(RTCScene scene, vec3 origen, vec3 destino) {
        RTCRayHit rayo;
        rayo.ray.org_x = origen.x;
        rayo.ray.org_y = origen.y;
        rayo.ray.org_z = origen.z;
        rayo.ray.dir_x = destino.x - origen.x;
        rayo.ray.dir_y = destino.y - origen.y;
        rayo.ray.dir_z = destino.z - origen.z;
        rayo.ray.tnear = 0.f;
        rayo.ray.tfar = std::numeric_limits<float>::infinity();
        rayo.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        RTCIntersectContext context;
        rtcInitIntersectContext(&context);
        rtcIntersect1(scene, &context, &rayo);

        return rayo;
    }
};

