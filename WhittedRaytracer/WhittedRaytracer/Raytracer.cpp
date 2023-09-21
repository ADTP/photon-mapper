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

void trazarFoton(RGBQUAD color, vec3 origen, vec3 direccion, Foton** listaFotones, int &fotonesEmitidos, int cantidadMaximaDeFotones) {
    Rayo* trazaFoton = new Rayo(origen, direccion, 0);
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
 
        RGBQUAD reflexionEspecular = elementoIntersectado->getCoeficienteReflexionEspecular();
        RGBQUAD reflexionDifusa = elementoIntersectado->getCoeficienteReflexionDifusa();
        
        uniform_real_distribution<double> distribution(0.0, 1.0);
        mt19937 engine;  //Mersenne twister MT19937
        auto generator = std::bind(distribution, engine);

        double a = generator();

        double dividendoDifusa = max({reflexionDifusa.rgbRed * color.rgbRed, reflexionDifusa.rgbGreen * color.rgbGreen, reflexionDifusa.rgbBlue * color.rgbBlue});
        double divisorDifusa = max({reflexionDifusa.rgbRed * color.rgbRed, reflexionDifusa.rgbGreen * color.rgbGreen, reflexionDifusa.rgbBlue * color.rgbBlue});
        double factorPotenciaDifusa = dividendoDifusa / divisorDifusa; // Pd

        double dividendoEspecular = max({ reflexionEspecular.rgbRed * color.rgbRed, reflexionEspecular.rgbGreen * color.rgbGreen, reflexionEspecular.rgbBlue * color.rgbBlue });
        double divisorEspecular = max({ reflexionEspecular.rgbRed * color.rgbRed, reflexionEspecular.rgbGreen * color.rgbGreen, reflexionEspecular.rgbBlue * color.rgbBlue });
        double factorPotenciaEspecular = dividendoEspecular / divisorEspecular; // Ps

        if (a < factorPotenciaDifusa) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.rgbRed / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.rgbGreen / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.rgbBlue / factorPotenciaDifusa };
            
            // QUESTION: HAY QUE ALMACENAR CADA UNO DE LOS REBOTES EN EL KDTREE?
            listaFotones[fotonesEmitidos] = new Foton(interseccionMasCercana, potenciaReflejada, 0, 0, 0); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
            fotonesEmitidos++;

            // TODO: CALCULAR DIRECCION
            vec3 direccionReflejada = { 0,0,0 };

            // QUESTION: HAY QUE CHECKEAR NO SUPERAR LA CANTIDAD MAXIMA DE FOTONES DE LA ESCENA ANTES DE TRAZAR REBOTES?
            // QUESTION: CORTAR RECURSION SI LA POTENCIA ES MUY PEQUEÑA?
            trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, fotonesEmitidos, cantidadMaximaDeFotones);

        } else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.rgbRed / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.rgbGreen / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.rgbBlue / factorPotenciaEspecular };
            
            listaFotones[fotonesEmitidos] = new Foton(interseccionMasCercana, potenciaReflejada, 0, 0, 0); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
            fotonesEmitidos++;

            // TODO: CALCULAR DIRECCION
            vec3 direccionReflejada = { 0,0,0 };
            trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, fotonesEmitidos, cantidadMaximaDeFotones);

        } else {
            listaFotones[fotonesEmitidos] = new Foton(interseccionMasCercana, color, 0, 0, 0); // FALTAN ANGULOS Y FLAG PARA KDTREE
            fotonesEmitidos++;
        }
    }
}

void generarMapaDeFotones() {
    Escena* escena = Escena::getInstance();

    int fotonesEmitidos = 0;
    vec3 dirFoton = { 0, 0, 0 };
    Foton** listaFotones = new Foton * [escena->cantidadDeFotones];

    uniform_real_distribution<double> distribution(-1.0, 1.0);
    mt19937 engine;  //Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);

    // Calcular la potencia total para poder ver cuanto aporta cada luz a la escena
    for (int i = 0; i < escena->luces.size(); i++) {}

    // Iterar sobre todas las luces de la escena emitiendo fotones
    for (int i = 0; i < escena->luces.size(); i++) {

        while (fotonesEmitidos != escena->cantidadDeFotones) {
            do {
                dirFoton.x = generator();
                dirFoton.y = generator();
                dirFoton.z = generator();
            } while (pow(dirFoton.x,2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);

            trazarFoton(escena->luces[i]->color, escena->luces[i]->posicion, dirFoton, listaFotones, fotonesEmitidos, escena->cantidadDeFotones);
            // listaFotones[fotonesEmitidos] = foton;

            // fotonesEmitidos++;
            // SCALE POWER OF STORED PHOTONS ??
        }
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
    
    // generarMapaDeFotones

    Whitted* whitted = new Whitted();

    // Whitted - recorrido de la maya tirando rayos desde la camara.
    for (int y = 0; y < pantalla->altura; y++) {
        for (int x = 0; x < pantalla->ancho; x++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[x][y], 1.00029f, x, y);
            RGBQUAD color = whitted->traza_RR(rayo, 0);
            FreeImage_SetPixelColor(pantalla->bitmap, x, y, &color);
        }
    }

    FreeImage_Save(FIF_PNG, pantalla->bitmap, "Final\\AA Resultado.png", 0);

    FreeImage_Save(FIF_PNG, pantalla->bitmapAmbiente,"Final\\Ambiente.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapDifuso, "Final\\Difuso.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapEspecular, "Final\\Especular.png", 0);

    FreeImage_Save(FIF_PNG, pantalla->bitmapReflexion, "Final\\Reflexion.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapTransmision, "Final\\Transmision.png", 0);

    FreeImage_DeInitialise();

    return 0;
}
