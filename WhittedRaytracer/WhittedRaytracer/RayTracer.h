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

class RayTracer
{
public:
    RGBQUAD trazaRR(RTCScene scene, RTCRayHit rayhit, Escena* escena) {
        //vec3 rayHitOrg = { rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z };
        //vec3 rayHitDir = { rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z };
        //vec3 interseccionMasCercana = rayHitOrg + rayHitDir * rayhit.ray.tfar;

        if (rayhit.ray.tfar != std::numeric_limits<float>::infinity()) {
            return sombraRR(scene, rayhit, escena->elementos[rayhit.hit.geomID], escena);
        }
       
        return negro;
    };
    
    RGBQUAD sombraRR(RTCScene scene, RTCRayHit rayoIncidente, Elemento* elementoIntersecado, Escena* escena) {
        vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
        vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
        vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
        
        vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
        normalRayoIncidente = normalize(normalRayoIncidente);

        /*float coeficenteAmbiente = elementoIntersecado->ambiente;
        RGBQUAD resultadoAmbiente = { 
            coeficenteAmbiente * elementoIntersecado->color.rgbRed, 
            coeficenteAmbiente * elementoIntersecado->color.rgbGreen, 
            coeficenteAmbiente * elementoIntersecado->color.rgbBlue
        };*/
        
        RGBQUAD resultadoFinal = negro;

        for (Luz* luz : escena->luces) {

            RTCRayHit rayoSombra = trazarRayo(scene, interseccionRayoIncidente + normalRayoIncidente * 0.1f, luz->posicion);
            vec3 normalInterseccion = { rayoSombra.hit.Ng_x, rayoSombra.hit.Ng_y, rayoSombra.hit.Ng_z };
            vec3 direccionRayoSombra = { rayoSombra.ray.dir_x , rayoSombra.ray.dir_y , rayoSombra.ray.dir_z };

            if (dot(normalInterseccion, direccionRayoSombra) > 0) { // Si la luz incide directo sobre el punto de interseccion
                float distanciaLuz = length(luz->posicion - interseccionRayoIncidente);

                if (rayoSombra.ray.tfar > distanciaLuz) { // Si no hay objetos intermedios
                    resultadoFinal.rgbRed += luz->color.rgbRed * (1 / distanciaLuz);
                    resultadoFinal.rgbGreen += luz->color.rgbGreen * (1 / distanciaLuz);
                    resultadoFinal.rgbBlue += luz->color.rgbBlue * (1 / distanciaLuz);
                    
                } else {
                    // PREGUNTAR SI HACE FALTA TENER EN CUENTA LA TRANSPARENCIA A ESTA ALTURA O SI SERIA SOLO CON LAS CAUSTICAS
                    
                    //vec3 origenSombra = { rayoSombra.ray.org_x, rayoSombra.ray.org_y, rayoSombra.ray.org_z };
                    //vec3 direccionSombra = { rayoSombra.ray.dir_x, rayoSombra.ray.dir_y, rayoSombra.ray.dir_z };
                    //vec3 interseccionRayoSombra = origenSombra + direccionSombra * rayoSombra.ray.tfar;
                    //RTCRayHit rayoSecundario = trazarRayo(scene, interseccionRayoSombra, luz->posicion);
                }
            }
        }

        resultadoFinal.rgbRed = std::min((int)resultadoFinal.rgbRed, 255) * elementoIntersecado->color.rgbRed / 255;
        resultadoFinal.rgbGreen = std::min((int)resultadoFinal.rgbGreen, 255) * elementoIntersecado->color.rgbGreen / 255;
        resultadoFinal.rgbBlue = std::min((int)resultadoFinal.rgbBlue, 255) * elementoIntersecado->color.rgbBlue / 255;

        return resultadoFinal;
    };

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

