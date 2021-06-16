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
   //}

RGBQUAD sombra_RR(int indiceObjetoIntersectado, Rayo* rayo, vec3 interseccion, vec3 normal, int profundidad) {
    Escena* escena = Escena::getInstance();
    Elemento* objeto = escena->elementos[indiceObjetoIntersectado];

    vec3 intensidadLuzAmbiente = { 0.6, 0.6, 0.6 };

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
        Rayo* rayoSecundario = new Rayo(interseccion, escena->luces[i]->posicion);

        float distanciaObjetoLuz = distance(interseccion, escena->luces[i]->posicion);
        vec3 intesidadLuzActual = escena->luces[i]->intesidad;

        float factorNormalLuz = dot(normal, rayoSecundario->direccion);

        if (factorNormalLuz > 0) {
            for (int j = 0; j < escena->elementos.size(); j++) {
                if (j != indiceObjetoIntersectado) {
                    float t = escena->elementos[j]->interseccionRayo(rayoSecundario);
                    vec3 interseccionSecundario = rayoSecundario->origen + rayoSecundario->direccion * t;

                    // Calcular cuánta luz es bloqueada por superficies opacas y transparentes
                    if (t > 0 && distance(interseccion, interseccionSecundario) < distanciaObjetoLuz) {
                        float coeficenteTransparencia = escena->elementos[j]->getTransparencia();
                        intesidadLuzActual.r = intesidadLuzActual.r * coeficenteTransparencia * escena->elementos[j]->color.rgbRed / 255;
                        intesidadLuzActual.g = intesidadLuzActual.r * coeficenteTransparencia * escena->elementos[j]->color.rgbGreen / 255;
                        intesidadLuzActual.b = intesidadLuzActual.r * coeficenteTransparencia * escena->elementos[j]->color.rgbBlue / 255;
                    }
                }
            }

            // usarlo para escalar los términos difusos y especulares antes de añadirlos a color;

            // calcular atenuacion de la luz en funcion de distancia luz objeto ????
            float coeficenteDifuso = objeto->getDifusa();
            resultadoDifuso.rgbRed += factorNormalLuz * coeficenteDifuso * objeto->color.rgbRed;
            resultadoDifuso.rgbGreen += factorNormalLuz * coeficenteDifuso * objeto->color.rgbGreen;
            resultadoDifuso.rgbBlue += factorNormalLuz * coeficenteDifuso * objeto->color.rgbBlue;

            float RV = dot(reflect(-rayoSecundario->direccion, normal), -rayo->direccion);  // puede q sea con - en ambos rayos o en uno solo
            if (RV > 0) { // si hay reflexion
                float coeficenteEspecular = objeto->getEspecular();
                resultadoEspecular.rgbRed += pow(RV,2) * coeficenteEspecular * objeto->color.rgbRed;
                resultadoEspecular.rgbGreen += pow(RV, 2) * coeficenteEspecular * objeto->color.rgbGreen;
                resultadoEspecular.rgbBlue += pow(RV, 2) * coeficenteEspecular * objeto->color.rgbBlue;
            }

            // AGREGAR FACTOR DE ATENUACION
            float factorAtenuacion = 1 / distanciaObjetoLuz;

            resultadoFinal.rgbRed += (resultadoDifuso.rgbRed + resultadoEspecular.rgbRed) * factorAtenuacion * intesidadLuzActual.r;
            resultadoFinal.rgbGreen += (resultadoDifuso.rgbGreen + resultadoEspecular.rgbGreen) * factorAtenuacion * intesidadLuzActual.g;
            resultadoFinal.rgbBlue += (resultadoDifuso.rgbBlue + resultadoEspecular.rgbBlue) * factorAtenuacion * intesidadLuzActual.b;

        }
    }

   // if (profundidad < profundidad_max) { /*Regresar si la prof. es excesiva */
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
    
    return resultadoFinal;
}

RGBQUAD traza_RR(Rayo* rayo_RR, int profundidad) {
    RGBQUAD color = { 0,0,0 };
    Escena* escena = Escena::getInstance();

    int indiceMasCerca = -1;    //elemento donde voy a guardar el mas chico
    float distancia = 100000;

    vec3 interseccionMasCercana;

    for (int i = 0; i < escena->elementos.size(); i++) {
        float t = escena->elementos[i]->interseccionRayo(rayo_RR);

        if (t != 0) {   // intersecta - PROBAR CON MAYOR Q CERO
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

int _tmain(int argc, _TCHAR* argv[])
{

    FreeImage_Initialise();

    // Creacion de imagenes con fecha y hora actual
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
    
    // Inicializacion de componentes    
    Camara* camara = new Camara({3, 3, 6}, {0, 1, 0}, {-1, 0, 0.8}, 40, 50, 1);
    
    Pantalla* pantalla = new Pantalla();
    pantalla->cargarMalla(camara);

    // Whitted - recorrido de la maya tirando rayos desde la camara.
    for (int i = pantalla->altura - 1; i >= 0; i--) {
        for (int j = 0; j < pantalla->ancho; j++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[i][j]);
            RGBQUAD color = traza_RR(rayo, 1);
            FreeImage_SetPixelColor(pantalla->bitmap, j, i, &color);

        }
    }

    FreeImage_Save(FIF_PNG, pantalla->bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
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
    //}
