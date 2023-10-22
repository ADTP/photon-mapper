#pragma once

#include "../Freeimage/FreeImage.h"
#include "nanoflann/nanoflann.h"
#include "nanoflann/utils.h"

#include "Escena.h"
#include "Foton.h"
#include "Pantalla.h"

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


class PhotonMapper {
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

    public:

    void generacionDeMapas(Escena* escena, PointCloud &mapaGlobal, PointCloud &mapaCausticas, RTCScene &scene) {
        if (escena->generarMapas) {
            generarMapaGlobal(escena, mapaGlobal, scene);
            generarMapaCausticas(escena, mapaCausticas, scene);

            // Guardar mapa global
            std::ofstream salidaGlobal("Mapas\\mapaGlobal.dat", std::ios::binary);
            for (const Foton& foton : mapaGlobal.pts) {
                foton.serializar(salidaGlobal);
            }
            salidaGlobal.close();

            // Guardar mapa causticas
            std::ofstream salidaCausticas("Mapas\\mapaCausticas.dat", std::ios::binary);
            for (const Foton& foton : mapaCausticas.pts) {
                foton.serializar(salidaCausticas);
            }
            salidaCausticas.close();

        }
        else {
            // Cargar mapa global
            std::ifstream entradaGlobal("Mapas\\mapaGlobal.dat", std::ios::binary);
            mapaGlobal.pts.clear();
            while (!entradaGlobal.eof()) {
                Foton foton;
                foton.deserializar(entradaGlobal);
                if (!entradaGlobal.eof()) {
                    mapaGlobal.pts.push_back(foton);
                }
            }
            entradaGlobal.close();

            // Cargar mapa causticas
            std::ifstream entradaCausticas("Mapas\\mapaCausticas.dat", std::ios::binary);
            mapaCausticas.pts.clear();
            while (!entradaCausticas.eof()) {
                Foton foton;
                foton.deserializar(entradaCausticas);
                if (!entradaCausticas.eof()) {
                    mapaCausticas.pts.push_back(foton);
                }
            }
            entradaCausticas.close();
        }
    }

    void generarMapaGlobal(Escena* escena, PointCloud& mapaGlobal, RTCScene& scene) {
        vec3 dirFoton = { 0, 0, 0 };

        // Calcular la potencia total para poder ver cuantos fotones aporta cada luz a la escena
        int potenciaTotal = 0;
        for (int i = 0; i < escena->luces.size(); i++) {
            potenciaTotal += escena->luces[i]->watts;
        }

        // Iterar sobre todas las luces de la escena emitiendo fotones
        for (int i = 0; i < escena->luces.size(); i++) {
            int fotonesAEmitir = escena->luces[i]->watts * escena->cantidadFotonesGlobal / potenciaTotal;
            int fotonesEmitidos = 0;

            //cout << "Fotones emitidos: " << fotonesEmitidos << "\n\n";
            while (fotonesEmitidos < fotonesAEmitir) {
                do {
                    dirFoton.x = generator2();
                    dirFoton.y = generator2();
                    dirFoton.z = generator2();
                } while (pow(dirFoton.x, 2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);
                
                trazarFoton(escena->luces[i]->color, escena->luces[i]->posicion, dirFoton, mapaGlobal, 0, scene);
                
                fotonesEmitidos++;
            }
        }
    }

    void trazarFoton(RGBQUAD color, vec3 origen, vec3 direccion, PointCloud& mapaGlobal, int profundidad, RTCScene &scene) {
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
            Elemento* elementoIntersecado = escena->elementos[rayhit.hit.geomID];
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
            if (dot(normalInterseccion, rayHitDir) < 0) {
                if (a < factorPotenciaDifusa) {
                    RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.r / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.g / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.b / factorPotenciaDifusa };
                    potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
                    if (profundidad > 0) {
                        vec3 colorVector;
                        colorVector.x = color.rgbRed;
                        colorVector.x = colorVector.x / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbRed / 255;
                        colorVector.y = color.rgbGreen;
                        colorVector.y = colorVector.y / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbGreen / 255;
                        colorVector.z = color.rgbBlue;
                        colorVector.z = colorVector.z / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbBlue / 255;

                        mapaGlobal.pts.push_back(Foton(interseccionMasCercana, colorVector, rayHitDir, 0, 0, 0));
                    }

                    vec3 centroEsfera = interseccionMasCercana + normalInterseccion;
                    vec3 direccionAletoriaEsfera;
                    do {
                        direccionAletoriaEsfera = { generator2(), generator2(), generator2() };
                    } while (pow(direccionAletoriaEsfera.x, 2) + pow(direccionAletoriaEsfera.y, 2) + pow(direccionAletoriaEsfera.z, 2) > 1);
                    vec3 puntoAleatorioEsfera = centroEsfera + direccionAletoriaEsfera;

                    trazarFoton(potenciaReflejada, interseccionMasCercana + 0.01f * normalInterseccion, puntoAleatorioEsfera - interseccionMasCercana, mapaGlobal, profundidad + 1, scene);

                }
                else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
                    RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.r / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.g / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.b / factorPotenciaEspecular };
                    potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
                    rayHitDir = normalize(rayHitDir);
                    direccionReflejada = reflect(rayHitDir, normalInterseccion);
                    //direccionReflejada = interseccionMasCercana - direccionReflejada;
                    trazarFoton(potenciaReflejada, interseccionMasCercana + normalInterseccion * 0.1f, direccionReflejada, mapaGlobal, profundidad + 1, scene);

                }
                else {
                    if (profundidad > 0 && length(elementoIntersecado->coeficienteReflexionEspecular) == 0 && elementoIntersecado->indiceRefraccion == 0) {
                        vec3 colorVector;
                        colorVector.x = color.rgbRed;
                        colorVector.x = colorVector.x / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbRed / 255;
                        colorVector.y = color.rgbGreen;
                        colorVector.y = colorVector.y / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbGreen / 255;
                        colorVector.z = color.rgbBlue;
                        colorVector.z = colorVector.z / (float)escena->cantidadFotonesGlobal * elementoIntersecado->color.rgbBlue / 255;
                        mapaGlobal.pts.push_back(Foton(interseccionMasCercana, colorVector, rayHitDir, 0, 0, 0));
                    }
                }
            }
        }
    }

    void generarMapaCausticas(Escena* escena, PointCloud& mapaCausticas, RTCScene& scene) {
        // Calcular la potencia total para poder ver cuantos fotones aporta cada luz a la escena
        int potenciaTotal = 0;
        for (int i = 0; i < escena->luces.size(); i++) {
            potenciaTotal += escena->luces[i]->watts;
        }

        // Iterar sobre todas las luces de la escena emitiendo fotones
        vec3 dirFoton = { 0, 0, 0 };
        for (int i = 0; i < escena->luces.size(); i++) {
            int fotonesAEmitir = escena->luces[i]->watts * escena->cantidadFotonesCausticas / potenciaTotal;
            int fotonesEmitidos = 0;

            //cout << "Fotones emitidos: " << fotonesEmitidos << "\n\n";
            while (fotonesEmitidos < fotonesAEmitir) {
                do {
                    dirFoton.x = generator2();
                    dirFoton.y = generator2();
                    dirFoton.z = generator2();
                } while (pow(dirFoton.x, 2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);

                // TIRAR RAYO EN LA DIRECCION DEFINIDA Y VER SI INTERSECTA CON UNO DE LOS OBJETOS ESPECULARES GUARDADOS
                RTCRayHit rayoPrueba;
                rayoPrueba.ray.org_x = escena->luces[i]->posicion.x;
                rayoPrueba.ray.org_y = escena->luces[i]->posicion.y;
                rayoPrueba.ray.org_z = escena->luces[i]->posicion.z;
                rayoPrueba.ray.dir_x = dirFoton.x;
                rayoPrueba.ray.dir_y = dirFoton.y;
                rayoPrueba.ray.dir_z = dirFoton.z;
                rayoPrueba.ray.tnear = 0.f;
                rayoPrueba.ray.tfar = std::numeric_limits<float>::infinity();
                rayoPrueba.hit.geomID = RTC_INVALID_GEOMETRY_ID;

                RTCIntersectContext context;
                rtcInitIntersectContext(&context);
                rtcIntersect1(scene, &context, &rayoPrueba);

                if (rayoPrueba.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                    Elemento* elementoIntersectado = escena->elementos[rayoPrueba.hit.geomID];

                    bool especular = (elementoIntersectado->coeficienteReflexionEspecular.r + elementoIntersectado->coeficienteReflexionEspecular.g + elementoIntersectado->coeficienteReflexionEspecular.b) > 0;
                    bool transparente = elementoIntersectado->indiceRefraccion > 0;

                    if (especular || transparente) {
                        vec3 colorLuz = { escena->luces[i]->color.rgbRed, escena->luces[i]->color.rgbGreen, escena->luces[i]->color.rgbBlue };
                        trazarFotonCaustica(rayoPrueba, mapaCausticas, escena, scene, colorLuz, 1);
                    }
                }

                fotonesEmitidos++;
            }
        }
    }

    void trazarFotonCaustica(RTCRayHit rayoIncidente, PointCloud& mapaCausticas, Escena* escena, RTCScene& scene, vec3 colorAcumulado, float indiceRefraccionActual) {
        vec3 origenRayo = { rayoIncidente.ray.org_x, rayoIncidente.ray.org_y, rayoIncidente.ray.org_z };
        vec3 direccionRayo = { rayoIncidente.ray.dir_x, rayoIncidente.ray.dir_y, rayoIncidente.ray.dir_z };
        vec3 interseccionRayo = origenRayo + direccionRayo * rayoIncidente.ray.tfar;
        direccionRayo = normalize(direccionRayo);
        vec3 normalInterseccion = { rayoIncidente.hit.Ng_x, rayoIncidente.hit.Ng_y, rayoIncidente.hit.Ng_z };
        normalInterseccion = normalize(normalInterseccion);
        vec3 normalRayoInvertida = dot(normalInterseccion, direccionRayo) <= 0 ? normalInterseccion : -normalInterseccion;


        if (rayoIncidente.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            Elemento* elementoIntersectado = escena->elementos[rayoIncidente.hit.geomID];
            bool especular = elementoIntersectado->coeficienteReflexionEspecular.r + elementoIntersectado->coeficienteReflexionEspecular.g + elementoIntersectado->coeficienteReflexionEspecular.b;
            bool transparente = elementoIntersectado->indiceRefraccion > 0;

            if (!especular && !transparente) {
                colorAcumulado.r = colorAcumulado.r / (float)escena->cantidadFotonesCausticas;
                colorAcumulado.g = colorAcumulado.g / (float)escena->cantidadFotonesCausticas;
                colorAcumulado.b = colorAcumulado.b / (float)escena->cantidadFotonesCausticas;
                mapaCausticas.pts.push_back(Foton(interseccionRayo, colorAcumulado, direccionRayo, 0, 0, 0));

            } else {

                if (transparente) { // transparencia perfecta
                    double rollAbsorcion = generator1();
                    if (rollAbsorcion > 0.1) {
                        float productoPuntoConNormal = dot(normalInterseccion, direccionRayo);
                        float indiceRefraccionProximo = productoPuntoConNormal > 0 ? 1.f : elementoIntersectado->indiceRefraccion;
                        float anguloIncidencia = acos(dot(normalInterseccion, direccionRayo)) * 180 / PI_PANTALLA;
                        if (anguloIncidencia > 90) { anguloIncidencia = anguloIncidencia - 90; }
                        float anguloCritico = asin(indiceRefraccionProximo / indiceRefraccionActual) * 180 / PI_PANTALLA;

                        if (indiceRefraccionProximo < indiceRefraccionActual && anguloIncidencia > anguloCritico) { // REFLEXION INTERNA TOTAL
                            vec3 direccionReflejada = reflect(direccionRayo, -normalInterseccion);
                            RTCRayHit rayoReflejado = trazarRayoConDireccion(scene, interseccionRayo + -normalInterseccion * 0.1f, direccionReflejada);
                            colorAcumulado.r = colorAcumulado.r * elementoIntersectado->color.rgbRed / 255;
                            colorAcumulado.g = colorAcumulado.g * elementoIntersectado->color.rgbGreen / 255;
                            colorAcumulado.b = colorAcumulado.b * elementoIntersectado->color.rgbBlue / 255;
                            trazarFotonCaustica(rayoReflejado, mapaCausticas, escena, scene, colorAcumulado, indiceRefraccionProximo);
                        }
                        else {
                            float eta = indiceRefraccionActual / indiceRefraccionProximo;
                            vec3 direccionRefractada = refract(direccionRayo, normalRayoInvertida, eta);
                            RTCRayHit rayoRefractado = trazarRayoConDireccion(scene, interseccionRayo + -normalRayoInvertida * 0.00001f, direccionRefractada);
                            colorAcumulado.r = colorAcumulado.r * elementoIntersectado->color.rgbRed/255;
                            colorAcumulado.g = colorAcumulado.g * elementoIntersectado->color.rgbGreen/255;
                            colorAcumulado.b = colorAcumulado.b * elementoIntersectado->color.rgbBlue/255;
                            trazarFotonCaustica(rayoRefractado, mapaCausticas, escena, scene, colorAcumulado, indiceRefraccionProximo);
                        }
                    }   
                }
                else { // reflexion perfecta
                    direccionRayo = normalize(direccionRayo);
                    vec3 direccionReflejada = reflect(direccionRayo, normalRayoInvertida);
                    RTCRayHit rayoReflejado = trazarRayoConDireccion(scene, interseccionRayo + normalRayoInvertida * 0.1f, direccionReflejada);
                    colorAcumulado.r = colorAcumulado.r * elementoIntersectado->color.rgbRed / 255;
                    colorAcumulado.g = colorAcumulado.g * elementoIntersectado->color.rgbGreen / 255;
                    colorAcumulado.b = colorAcumulado.b * elementoIntersectado->color.rgbBlue / 255;
                    trazarFotonCaustica(rayoReflejado, mapaCausticas, escena, scene, colorAcumulado, 1.0f);
                    //vec3 direccionReflejada = reflect(direccionRayo, normalInterseccion);

                    //RTCRayHit rayoReflejado;
                    //rayoReflejado.ray.org_x = interseccionRayo.x;
                    //rayoReflejado.ray.org_y = interseccionRayo.y;
                    //rayoReflejado.ray.org_z = interseccionRayo.z;
                    //rayoReflejado.ray.dir_x = direccionReflejada.x;
                    //rayoReflejado.ray.dir_y = direccionReflejada.y;
                    //rayoReflejado.ray.dir_z = direccionReflejada.z;
                    //rayoReflejado.ray.tnear = 0.f;
                    //rayoReflejado.ray.tfar = std::numeric_limits<float>::infinity();
                    //rayoReflejado.hit.geomID = RTC_INVALID_GEOMETRY_ID;

                    //RTCIntersectContext context;
                    //rtcInitIntersectContext(&context);
                    //rtcIntersect1(scene, &context, &rayoReflejado);

                    //// LA REFLEXION PERFECTA NO DEBERIA CAMBIAR EL COLOR DE LA LUZ, PERO SI TENER EN CUENTA SI VIENE CAMBIADA POR UN OBJ TRANSPARENTE ANTERIOR
                    ////colorAcumulado.r += elementoIntersectado->color.rgbRed * elementoIntersectado->coeficienteReflexionEspecular.r;
                    ////colorAcumulado.g += elementoIntersectado->color.rgbGreen * elementoIntersectado->coeficienteReflexionEspecular.g;
                    ////colorAcumulado.b += elementoIntersectado->color.rgbBlue * elementoIntersectado->coeficienteReflexionEspecular.b;

                    //trazarFotonCaustica(rayoReflejado, mapaCausticas, escena, scene, colorAcumulado, indiceRefraccionActual);
                }
            }
        }
    }
};

