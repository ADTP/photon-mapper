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

int PROFUNDIDAD_MAXIMA = 5;

using CustomKDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3>;

class RayTracer {
    public:
    
    RGBQUAD iluminacionDirecta(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
            Elemento* elementoIntersecado = escena->elementos[rayoIncidente.hit.geomID];
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
            normalRayoIncidente = normalize(normalRayoIncidente);
            vec3 resultadoIntermedio = { 0,0, 0 };
            RGBQUAD resultadoFinal = negro;
            if (elementoIntersecado->indiceRefraccion == 0 && length(elementoIntersecado->coeficienteReflexionEspecular) == 0) {
                for (Luz* luz : escena->luces) {
                    int cantidadDeRayos = 300;
                    for (int i = 0; i < cantidadDeRayos; i++) {
                        vec3 offsetLuz;
                        do {
                            offsetLuz.x = generator2() * 0.1;
                            offsetLuz.y = generator2() * 0;
                            offsetLuz.z = generator2() * 0.1;
                        } while (pow(offsetLuz.x, 2) + pow(offsetLuz.y, 2) + pow(offsetLuz.z, 2) > 1);

                        RTCRayHit rayoSombra = trazarRayo(scene, interseccionRayoIncidente + normalRayoIncidente * 0.1f, luz->posicion + offsetLuz);
                        vec3 origenRayoSombra = { rayoSombra.ray.org_x, rayoSombra.ray.org_y, rayoSombra.ray.org_z };
                        vec3 direccionRayoSombra = { rayoSombra.ray.dir_x , rayoSombra.ray.dir_y , rayoSombra.ray.dir_z };

                        if (dot(normalRayoIncidente, direccionRayoSombra) > 0) { // Si la luz incide directo sobre el punto de interseccion
                            float distanciaLuz = length(luz->posicion + offsetLuz - interseccionRayoIncidente);
                            float distanciaInterseccion = length((origenRayoSombra + direccionRayoSombra * rayoSombra.ray.tfar) - origenRayoSombra);

                            if (distanciaInterseccion > distanciaLuz) { // Si no hay objetos intermedios
                                float escala = dot(normalize(normalRayoIncidente), normalize(direccionRayoSombra));
                                resultadoIntermedio.x = std::min(255.f, (resultadoIntermedio.x + luz->color.rgbRed * (1 / distanciaLuz) * escala * luz->watts / (20 * cantidadDeRayos)));
                                resultadoIntermedio.y = std::min(255.f, (resultadoIntermedio.y + luz->color.rgbGreen * (1 / distanciaLuz) * escala * luz->watts / (20 * cantidadDeRayos)));
                                resultadoIntermedio.z = std::min(255.f, (resultadoIntermedio.z + luz->color.rgbBlue * (1 / distanciaLuz) * escala * luz->watts / (20 * cantidadDeRayos)));

                            }
                        }
                    }
                    resultadoFinal.rgbRed = trunc(resultadoIntermedio.x);
                    resultadoFinal.rgbGreen = trunc(resultadoIntermedio.y);
                    resultadoFinal.rgbBlue = trunc(resultadoIntermedio.z);

                }
            }
            resultadoFinal.rgbRed = std::min((int)resultadoFinal.rgbRed, 255) * elementoIntersecado->color.rgbRed / 255;
            resultadoFinal.rgbGreen = std::min((int)resultadoFinal.rgbGreen, 255) * elementoIntersecado->color.rgbGreen / 255;
            resultadoFinal.rgbBlue = std::min((int)resultadoFinal.rgbBlue, 255) * elementoIntersecado->color.rgbBlue / 255;
            return resultadoFinal;
        }

        return negro;
    };

    RGBQUAD iluminacionIndirecta(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena, PointCloud mapaGlobal, CustomKDTree* index) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
            
            Elemento* elementoIntersecado = escena->elementos[rayoIncidente.hit.geomID];
            
            const float consulta[3] = { interseccionRayoIncidente.x, interseccionRayoIncidente.y, interseccionRayoIncidente.z };
            if (elementoIntersecado->indiceRefraccion == 0 && length(elementoIntersecado->coeficienteReflexionEspecular) == 0) {
                int knn = 500;
                std::vector<uint32_t> ret_index(knn);
                std::vector<float>    out_dist_sqr(knn);
                index->knnSearch(&consulta[0], knn, &ret_index[0], &out_dist_sqr[0]);
                int cantFotonesResultantes = knn;

                if (cantFotonesResultantes > 0) {
                    vec3 flujoAcumulado = { 0,0,0 };

                    for (int i = 0; i < ret_index.size(); i++) {
                        Foton foton = mapaGlobal.pts[ret_index[i]];

                        float productoPunto = dot(normalize(foton.direccionIncidente), normalize(-normalRayoIncidente));

                        if (productoPunto > 0) {
                            flujoAcumulado.r += foton.potencia.r * 1500;
                            flujoAcumulado.g += foton.potencia.g * 1500;
                            flujoAcumulado.b += foton.potencia.b * 1500;
                        }
                    }

                    int minFotones = std::min(knn, (int)mapaGlobal.pts.size());

                    float distanciaFotonMasLejano = length(mapaGlobal.pts[ret_index[minFotones - 1]].posicion - interseccionRayoIncidente);
                    float pi = 3.14159265358979323846;

                    RGBQUAD resultado;
                    resultado.rgbRed = std::min((int)(flujoAcumulado.r * (elementoIntersecado->color.rgbRed / 255) / (pi * pow(distanciaFotonMasLejano, 2))), 255);
                    resultado.rgbGreen = std::min((int)(flujoAcumulado.g * (elementoIntersecado->color.rgbGreen / 255) / (pi * pow(distanciaFotonMasLejano, 2))), 255);
                    resultado.rgbBlue = std::min((int)(flujoAcumulado.b * (elementoIntersecado->color.rgbBlue / 255) / (pi * pow(distanciaFotonMasLejano, 2))), 255);

                    return resultado;
                }
            }
        }

        return negro;
    }

    RGBQUAD iluminacionCausticas(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena, PointCloud mapaCausticas, CustomKDTree* index) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
            Elemento* elementoIntersecado = escena->elementos[rayoIncidente.hit.geomID];
            
            const float consulta[3] = { interseccionRayoIncidente.x, interseccionRayoIncidente.y, interseccionRayoIncidente.z };
            const float radioEsfera = 30;
            std::vector <nanoflann::ResultItem<uint32_t, float>> fotonesResultantes;

            nanoflann::SearchParameters params;
            params.sorted = true;
            index->radiusSearch(&consulta[0], radioEsfera, fotonesResultantes, params);
            
            int knn = 500;
            int iMaximo = 0;

            if (fotonesResultantes.size() > 0) {
                vec3 flujoAcumulado = { 0,0,0 };

                // Tomo los fotones de una esfera. Si hay más de KNN fotones, me quedo con los KNN más cercanos.
                for (int i = 0; i < fotonesResultantes.size() && i < knn; i++) {
                    iMaximo = i;
                    Foton foton = mapaCausticas.pts[fotonesResultantes[i].first];
                    flujoAcumulado.r += foton.potencia.r * 500;
                    flujoAcumulado.g += foton.potencia.g * 500;
                    flujoAcumulado.b += foton.potencia.b * 500;
                }
                float distanciaFotonMasLejano = length(mapaCausticas.pts[fotonesResultantes[std::min(iMaximo,knn)].first].posicion - interseccionRayoIncidente);
                float pi = 3.14159265358979323846;

                RGBQUAD resultado;
                float valor = flujoAcumulado.r / (pi * pow(distanciaFotonMasLejano, 8));
                resultado.rgbRed = std::min((int)(flujoAcumulado.r * elementoIntersecado->color.rgbRed / 255 / (pi * pow(distanciaFotonMasLejano, 2))), 255);
                resultado.rgbGreen = std::min((int)(flujoAcumulado.g * elementoIntersecado->color.rgbGreen / 255 / (pi * pow(distanciaFotonMasLejano, 2))), 255);
                resultado.rgbBlue = std::min((int)(flujoAcumulado.b * elementoIntersecado->color.rgbBlue / 255 / (pi * pow(distanciaFotonMasLejano, 2))), 255);

                return resultado;
            }
        }

        return negro;
    }

    RGBQUAD iluminacionEspecular(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena, int profundidadActual, float indiceRefraccionActual, PointCloud mapaGlobal, CustomKDTree* indexGlobal, PointCloud mapaCausticas, CustomKDTree* indexCausticas) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 direccionRayo = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + direccionRayo * rayoIncidente.ray.tfar;
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
            normalRayoIncidente = normalize(normalRayoIncidente);
            vec3 normalRayoInvertida = dot(normalRayoIncidente, direccionRayo) <= 0 ? normalRayoIncidente : -normalRayoIncidente;
            direccionRayo = normalize(direccionRayo);

            RGBQUAD resultadoFinal = negro;


            Elemento* elementoIntersecado = escena->elementos[rayoIncidente.hit.geomID];
            bool especular = elementoIntersecado->coeficienteReflexionEspecular.r + elementoIntersecado->coeficienteReflexionEspecular.g + elementoIntersecado->coeficienteReflexionEspecular.b;
            bool transparente = elementoIntersecado->indiceRefraccion > 0;

            if (profundidadActual < PROFUNDIDAD_MAXIMA && (especular||transparente)) {
                if (especular) {

                    vec3 direccionReflejada = reflect(direccionRayo, normalRayoIncidente);
                    RTCRayHit rayoReflejado = trazarRayoConDireccion(scene, interseccionRayoIncidente + normalRayoIncidente * 0.1f, direccionReflejada);
                    RGBQUAD colorReflejado = iluminacionEspecular(scene, rayoReflejado, escena, profundidadActual + 1, indiceRefraccionActual, mapaGlobal, indexGlobal, mapaCausticas, indexCausticas);

                    resultadoFinal.rgbRed = std::min(255.f, resultadoFinal.rgbRed + colorReflejado.rgbRed * elementoIntersecado->coeficienteReflexionEspecular.r);
                    resultadoFinal.rgbGreen = std::min(255.f, resultadoFinal.rgbGreen + colorReflejado.rgbGreen * elementoIntersecado->coeficienteReflexionEspecular.g);
                    resultadoFinal.rgbBlue = std::min(255.f, resultadoFinal.rgbBlue + colorReflejado.rgbBlue * elementoIntersecado->coeficienteReflexionEspecular.b);

                    return resultadoFinal;
                }
                else {

                    float productoPuntoConNormal = dot(normalRayoIncidente, direccionRayo);
                    float indiceRefraccionProximo = productoPuntoConNormal > 0 ? 1.f : elementoIntersecado->indiceRefraccion;
                    float anguloIncidencia = acos(productoPuntoConNormal) * 180 / PI_PANTALLA;
                    //if (anguloIncidencia > 90) { anguloIncidencia = anguloIncidencia - 90; }

                    float anguloCritico = asin(indiceRefraccionProximo / indiceRefraccionActual) * 180 / PI_PANTALLA;
                    
                    if (indiceRefraccionProximo < indiceRefraccionActual && anguloIncidencia > anguloCritico) { // REFLEXION INTERNA TOTAL
                        vec3 direccionReflejada = reflect(direccionRayo, normalRayoInvertida);
                        RTCRayHit rayoReflejado = trazarRayoConDireccion(scene, interseccionRayoIncidente + normalRayoInvertida * 0.01f, direccionReflejada);
                        RGBQUAD colorReflejado = iluminacionEspecular(scene, rayoReflejado, escena, profundidadActual + 1, indiceRefraccionActual, mapaGlobal, indexGlobal, mapaCausticas, indexCausticas);

                        resultadoFinal.rgbRed = std::min(255.f, resultadoFinal.rgbRed + colorReflejado.rgbRed * elementoIntersecado->coeficienteReflexionEspecular.r);
                        resultadoFinal.rgbGreen = std::min(255.f, resultadoFinal.rgbGreen + colorReflejado.rgbGreen * elementoIntersecado->coeficienteReflexionEspecular.g);
                        resultadoFinal.rgbBlue = std::min(255.f, resultadoFinal.rgbBlue + colorReflejado.rgbBlue * elementoIntersecado->coeficienteReflexionEspecular.b);

                        return resultadoFinal;

                    } else {

                        float eta = indiceRefraccionActual / indiceRefraccionProximo;
                      
                        vec3 direccionRefractada = refract(direccionRayo, normalRayoInvertida, eta);
                        RTCRayHit rayoRefractado = trazarRayoConDireccion(scene, interseccionRayoIncidente + -normalRayoInvertida * 0.1f, direccionRefractada);
                        RGBQUAD colorRefractado = iluminacionEspecular(scene, rayoRefractado, escena, profundidadActual + 1, indiceRefraccionProximo, mapaGlobal, indexGlobal, mapaCausticas, indexCausticas);
                        resultadoFinal = elementoIntersecado->color;
                        resultadoFinal.rgbRed = std::min(255, resultadoFinal.rgbRed * colorRefractado.rgbRed / 255);
                        resultadoFinal.rgbGreen = std::min(255, resultadoFinal.rgbGreen * colorRefractado.rgbGreen / 255);
                        resultadoFinal.rgbBlue = std::min(255, resultadoFinal.rgbBlue * colorRefractado.rgbBlue / 255);

                        return resultadoFinal;
                    }
                }
            }

            if (profundidadActual >= 1) {
                resultadoFinal.rgbRed = std::min(iluminacionDirecta(scene, rayoIncidente, escena).rgbRed + iluminacionIndirecta(scene, rayoIncidente, escena, mapaGlobal, indexGlobal).rgbRed + iluminacionCausticas(scene, rayoIncidente, escena, mapaCausticas, indexCausticas).rgbRed, 255);
                resultadoFinal.rgbGreen = std::min(iluminacionDirecta(scene, rayoIncidente, escena).rgbGreen + iluminacionIndirecta(scene, rayoIncidente, escena, mapaGlobal, indexGlobal).rgbGreen + iluminacionCausticas(scene, rayoIncidente, escena, mapaCausticas, indexCausticas).rgbGreen, 255);
                resultadoFinal.rgbBlue = std::min(iluminacionDirecta(scene, rayoIncidente, escena).rgbBlue + iluminacionIndirecta(scene, rayoIncidente, escena, mapaGlobal, indexGlobal).rgbBlue + iluminacionCausticas(scene, rayoIncidente, escena, mapaCausticas, indexCausticas).rgbBlue, 255);
                return resultadoFinal;
            }
        }

        return negro;
    };

    RGBQUAD imagenMapaFotones(RTCScene scene, RTCRayHit rayoIncidente, Escena* escena, PointCloud mapaGlobal, CustomKDTree* index, int cantidadFotones) {
        if (rayoIncidente.ray.tfar != std::numeric_limits<float>::infinity()) {
            vec3 rayHitOrg = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
            vec3 rayHitDir = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
            vec3 interseccionRayoIncidente = rayHitOrg + rayHitDir * rayoIncidente.ray.tfar;
            vec3 normalRayoIncidente = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };

            const float consulta[3] = { interseccionRayoIncidente.x, interseccionRayoIncidente.y, interseccionRayoIncidente.z };
            const float radioEsfera = 0.0001;
            std::vector <nanoflann::ResultItem<uint32_t, float>> fotonesResultantes;

            nanoflann::SearchParameters params;
            params.sorted = true;
            index->radiusSearch(&consulta[0], radioEsfera, fotonesResultantes, params);

            if (fotonesResultantes.size() > 0) {
                vec3 flujoAcumulado = { 0,0,0 };
                Foton foton = mapaGlobal.pts[fotonesResultantes[0].first];
                RGBQUAD resultado = { foton.potencia.b * cantidadFotones, foton.potencia.g * cantidadFotones, foton.potencia.r * cantidadFotones };
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

    RTCRayHit trazarRayoConDireccion(RTCScene scene, vec3 origen, vec3 direccion) {
        RTCRayHit rayo;
        rayo.ray.org_x = origen.x;
        rayo.ray.org_y = origen.y;
        rayo.ray.org_z = origen.z;
        rayo.ray.dir_x = direccion.x;
        rayo.ray.dir_y = direccion.y;
        rayo.ray.dir_z = direccion.z;
        rayo.ray.tnear = 0.f;
        rayo.ray.tfar = std::numeric_limits<float>::infinity();
        rayo.hit.geomID = RTC_INVALID_GEOMETRY_ID;

        RTCIntersectContext context;
        rtcInitIntersectContext(&context);
        rtcIntersect1(scene, &context, &rayo);

        return rayo;
    }
};

