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
#include "Whitted.h"
#include "Foton.h"

#include <vector>
#include <iostream>
#include <string>
#include <random> 
#include <functional> 
#include <list> 

using namespace std;

const char* nombreImagen(const char* identificador) {
    // Creacion de imagenes con fecha y hora actual
    time_t rawtime;
    struct tm timeinfo;
    char buffer[80];

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    strftime(buffer, sizeof(buffer), "Alternativas\\%d %m %Y %H %M %S", &timeinfo);
    string str(buffer);

    const int n = str.length();
    char char_array[60];
    strcpy_s(char_array, str.c_str());

    strcat_s(char_array, identificador);

    return char_array;
}

void trazarFoton(RGBQUAD color, vec3 origen, vec3 direccion, vector<Foton> &listaFotones, int profundidad, float indiceRefraccionActual) {
    Rayo* trazaFoton = new Rayo(origen, direccion, indiceRefraccionActual);
    Escena* escena = Escena::getInstance();

    int indiceMasCerca = -1;
    float distancia = 100000;

    vec3 interseccionMasCercana;

    for (int i = 0; i < escena->elementos.size(); i++) {
        float t = escena->elementos[i]->interseccionRayo(trazaFoton);

        if (t > 1e-3) {
            vec3 interseccion = trazaFoton->origen + trazaFoton->direccion * t;
            float distanciaNueva = distance(trazaFoton->origen, interseccion);

            if (distanciaNueva < distancia) {
                distancia = distanciaNueva;
                indiceMasCerca = i;
                interseccionMasCercana = interseccion;
            }
        }
    }

    // si hay objeto intersectado
    if (indiceMasCerca != -1) {
        Elemento* elementoIntersectado = escena->elementos[indiceMasCerca];
        vec3 normalInterseccion = elementoIntersectado->normalDelPunto(interseccionMasCercana);
        vec3 direccionIncidente = direccion - origen;

        RGBQUAD reflexionEspecular = elementoIntersectado->getCoeficienteReflexionEspecular();
        RGBQUAD reflexionDifusa = elementoIntersectado->getCoeficienteReflexionDifusa();
        
        uniform_real_distribution<double> distribution(0.0, 1.0);
        mt19937 engine;  //Mersenne twister MT19937
        auto generator = std::bind(distribution, engine);

        double a = generator();

        double dividendoDifusa = max({reflexionDifusa.rgbRed * color.rgbRed, reflexionDifusa.rgbGreen * color.rgbGreen, reflexionDifusa.rgbBlue * color.rgbBlue});
        double divisorDifusa = max({color.rgbRed, color.rgbGreen, color.rgbBlue});
        double factorPotenciaDifusa = dividendoDifusa / divisorDifusa; // Pd

        double dividendoEspecular = max({ reflexionEspecular.rgbRed * color.rgbRed, reflexionEspecular.rgbGreen * color.rgbGreen, reflexionEspecular.rgbBlue * color.rgbBlue });
        double divisorEspecular = max({ color.rgbRed, color.rgbGreen, color.rgbBlue });
        double factorPotenciaEspecular = dividendoEspecular / divisorEspecular; // Ps

        vec3 direccionReflejada{};
        if (a < factorPotenciaDifusa) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.rgbRed / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.rgbGreen / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.rgbBlue / factorPotenciaDifusa };
            
            if (profundidad >= 1) {
                listaFotones.push_back(Foton(interseccionMasCercana, potenciaReflejada, 0, 0, 0)); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
            } 
            
            do {
                direccionReflejada = { generator(), generator(), generator() };
            } while (dot(normalInterseccion, direccionReflejada) <= 0);

            trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, profundidad++, trazaFoton->refraccionObjetoActual);

        } else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.rgbRed / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.rgbGreen / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.rgbBlue / factorPotenciaEspecular };

            float refraccionProximoRayo = elementoIntersectado->getRefraccion();
            if (elementoIntersectado->getReflexion() > 0.0) { // si hay que reflejar
                direccionReflejada = reflect(direccionIncidente, normalInterseccion);

            }
            else if (elementoIntersectado->getRefraccion() > 0.0) { // si hay que refractar
                if (dot(normalInterseccion, direccionIncidente) > 0) {
                    refraccionProximoRayo = 1.00029;
                }

                direccionReflejada = refract(direccionIncidente, normalInterseccion, trazaFoton->refraccionObjetoActual / refraccionProximoRayo);
            } 

            trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, profundidad++, refraccionProximoRayo);

        } else {
            if (elementoIntersectado->getDifusa() > 0.0) {
                listaFotones.push_back(Foton(interseccionMasCercana, color, 0, 0, 0));
            }
        }
    }
}

void generarMapaDeFotones() {
    Escena* escena = Escena::getInstance();

    vec3 dirFoton = { 0, 0, 0 };
    vector<Foton> listaFotones; // TODO: SERIA MEJOR USAR STD::VECTOR DE UNA? 

    uniform_real_distribution<double> distribution(-1.0, 1.0);
    mt19937 engine;  //Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);

    // Calcular la potencia total para poder ver cuantos fotones aporta cada luz a la escena
    int potenciaTotal = 0;
    for (int i = 0; i < escena->luces.size(); i++) {
        potenciaTotal += escena->luces[i]->watts;
    }

    // Iterar sobre todas las luces de la escena emitiendo fotones
    for (int i = 0; i < escena->luces.size(); i++) {
        int fotonesAEmitir = escena->luces[i]->watts * escena->cantidadDeFotones / potenciaTotal;
        int fotonesEmitidos = 0;

        while (fotonesEmitidos < fotonesAEmitir) {
            do {
                dirFoton.x = generator();
                dirFoton.y = generator();
                dirFoton.z = generator();
            } while (pow(dirFoton.x,2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);

            trazarFoton(escena->luces[i]->color, escena->luces[i]->posicion, dirFoton, listaFotones, 0, 1.00029);
            fotonesEmitidos++;
            // QUESTION: SCALE POWER OF STORED PHOTONS ??
        }
    }

    kdt::KDTree<Foton> mapaDeFotonesGlobal(listaFotones);
    
    cout << listaFotones.size();

    int k = 10;
    std::vector<int> indices = mapaDeFotonesGlobal.knnSearch(listaFotones.front(), k);
    for (int i = 0; i < indices.size(); i++) {
        cout << indices[i];
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    FreeImage_Initialise();

    Escena* escena = Escena::getInstance();
    Camara* camara = Camara::getInstance();

    Pantalla* pantalla = Pantalla::getInstance();
    pantalla->cargarMalla(camara);

    // AGREGAR MAPA DE PROYECCIONES
    
    generarMapaDeFotones();

    //Whitted* whitted = new Whitted();

    //// Whitted - recorrido de la maya tirando rayos desde la camara.
    //for (int y = 0; y < pantalla->altura; y++) {
    //    for (int x = 0; x < pantalla->ancho; x++) {
    //        Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[x][y], 1.00029f, x, y);
    //        RGBQUAD color = whitted->traza_RR(rayo, 0);
    //        FreeImage_SetPixelColor(pantalla->bitmap, x, y, &color);
    //    }
    //}

    //FreeImage_Save(FIF_PNG, pantalla->bitmap, "Final\\AA Resultado.png", 0);

    //FreeImage_Save(FIF_PNG, pantalla->bitmapAmbiente,"Final\\Ambiente.png", 0);
    //FreeImage_Save(FIF_PNG, pantalla->bitmapDifuso, "Final\\Difuso.png", 0);
    //FreeImage_Save(FIF_PNG, pantalla->bitmapEspecular, "Final\\Especular.png", 0);

    //FreeImage_Save(FIF_PNG, pantalla->bitmapReflexion, "Final\\Reflexion.png", 0);
    //FreeImage_Save(FIF_PNG, pantalla->bitmapTransmision, "Final\\Transmision.png", 0);

    //FreeImage_DeInitialise();

    return 0;
}
