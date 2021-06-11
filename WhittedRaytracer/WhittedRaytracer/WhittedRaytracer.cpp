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

using namespace std;

RGBQUAD traza_RR(Rayo* rayo_RR, int profundidad) {
    RGBQUAD color = { 0,0,0 };
    Escena* escena = Escena::getInstance();
    int indiceMasCerca = -1;//elemento donde voy a guardar el mas chico
    float distancia = 100000;//bien grandota
    vec3 interseccionMasCercana;
    for (int i = 0;i < escena->elementos.size();i++) {
        float t = escena->elementos[i]->interseccionRayo(rayo_RR);
        if (t != -1) {// intersecta
            vec3 interseccion = rayo_RR->origen + rayo_RR->direccion * t;
            float distanciaNueva = distance(rayo_RR->origen, interseccion);
            if (distanciaNueva < distancia) {
                distancia = distanciaNueva;
                indiceMasCerca = i;
                interseccionMasCercana = interseccion;
            }
        }
    }
    if (indiceMasCerca != -1) {//hay objeto intersectado
        //calculamos normal de la interseccion.
        vec3 normal = escena->elementos[indiceMasCerca]->normalDelPunto(interseccionMasCercana);
        //LLAMO A SOMBRA.
        color = escena->elementos[indiceMasCerca]->color;
    }
    return color;
}
int _tmain(int argc, _TCHAR* argv[])
{
    int width = 640;
    int height = 480;

    FreeImage_Initialise();
    FIBITMAP *bitmap = FreeImage_Allocate(width, height, 24);

    time_t rawtime;
    struct tm timeinfo;
    char buffer[80];

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    strftime(buffer, sizeof(buffer), "Historial\\Resultado %d %m %Y %H %M %S.png", &timeinfo);
    string str(buffer);
    
    const int n = str.length();
    char char_array[44];
    strcpy_s(char_array, str.c_str());
    
    RGBQUAD color;
    color.rgbRed = 255;
    color.rgbGreen = 234;
    color.rgbBlue = 123;
    
    /* Whitted */
    Pantalla* pantalla = new Pantalla();
    Escena* escena = new Escena();
    Camara* camara = new Camara({300, 200, 0}, { 0, 1, 0 }, { 300, 200, 100 }, 0, 0, 60, 90);


    for (int i = 0; i < pantalla->ancho; i++) {
        for (int j = 0; j < pantalla->altura; j++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[i][j]);
            RGBQUAD color=traza_RR(rayo, 1);
            FreeImage_SetPixelColor(pantalla->bitmap, i, j, &color);

        }
    }
    
    //// Seleccionar el centro de proyección y la ventana en el plano de vista;
    //for (cada línea de barrido en la imagen) {
    //    for (cada píxel en la línea de barrido) {
    //        determinar rayo por centro de proyección y píxel;
    //        píxel = traza_RR(rayo, 1);
    //    }
    //}

    //// Intersecar rayo con los objetos y calcular la sombra en la intersección más cercana.
    //// La profundidad es la profundidad actual en el árbol de rayos */

    //color_RR traza_RR(rayo_RR, int profundidad)
    //{
    //    determinar la intersección más cercana de rayo con un objeto;
    //    if (Hay objeto intersecado) {
    //        calcular la normal en la intersección;
    //        return sombra_RR(obj.intersecado más cercano, rayo, intersección, normal, profundidad);
    //    }
    //    else {
    //        return VALOR_FONDO
    //    }
    //}

    //// Calcular sombra en un punto, con rayos para sombras, reflex. y refrac. */

    //color_RR sombra_RR(objeto, rayo, punto, normal, int profundidad) {
    //    color = término del ambiente;
    //    for (cada luz) {
    //        rayo_s = rayo desde el punto a la luz;
    //        if (producto punto entre normal y dirección de la luz es positivo) {
    //            Calcular cuánta luz es bloqueada por sup.opacas y transp., y
    //                usarlo para escalar los términos difusos y especulares antes de
    //                añadirlos a color;
    //        }
    //    }

    //    if (profundidad < profundidad_max) { /*Regresar si la prof. es excesiva */
    //        if (objeto es reflejante) {
    //            rayo_r = rayo en la dirección de reflexión desde punto;
    //            color_r = traza_RR(rayo_r, profundidad + 1);
    //            escalar color_r por el coeficiente especular y añadir a color;
    //        }
    //        if (objeto es transparente) {
    //            if (no ocurre la reflexión interna total) {
    //                rayo_t = rayo en la dirección de refracción desde punto;
    //                color_t = traza_RR(rayo_t, profundidad + 1);
    //                escalar color_t por el coeficiente de transmisión y
    //                    añadir a color;
    //            }
    //        }
    //    }

    //    return color; /* Devolver color del rayo. */
    

    FreeImage_Save(FIF_PNG, bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
}

