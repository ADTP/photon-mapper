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


    for (int a = 0; a < pantalla->ancho; a++) {
        for (int l = 0; l < pantalla->altura; l++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[a][l]);

        }
    }

    //// Seleccionar el centro de proyecci�n y la ventana en el plano de vista;
    //for (cada l�nea de barrido en la imagen) {
    //    for (cada p�xel en la l�nea de barrido) {
    //        determinar rayo por centro de proyecci�n y p�xel;
    //        p�xel = traza_RR(rayo, 1);
    //    }
    //}

    //// Intersecar rayo con los objetos y calcular la sombra en la intersecci�n m�s cercana.
    //// La profundidad es la profundidad actual en el �rbol de rayos */

    //color_RR traza_RR(rayo_RR, int profundidad)
    //{
    //    determinar la intersecci�n m�s cercana de rayo con un objeto;
    //    if (Hay objeto intersecado) {
    //        calcular la normal en la intersecci�n;
    //        return sombra_RR(obj.intersecado m�s cercano, rayo, intersecci�n, normal, profundidad);
    //    }
    //    else {
    //        return VALOR_FONDO
    //    }
    //}

    //// Calcular sombra en un punto, con rayos para sombras, reflex. y refrac. */

    //color_RR sombra_RR(objeto, rayo, punto, normal, int profundidad) {
    //    color = t�rmino del ambiente;
    //    for (cada luz) {
    //        rayo_s = rayo desde el punto a la luz;
    //        if (producto punto entre normal y direcci�n de la luz es positivo) {
    //            Calcular cu�nta luz es bloqueada por sup.opacas y transp., y
    //                usarlo para escalar los t�rminos difusos y especulares antes de
    //                a�adirlos a color;
    //        }
    //    }

    //    if (profundidad < profundidad_max) { /*Regresar si la prof. es excesiva */
    //        if (objeto es reflejante) {
    //            rayo_r = rayo en la direcci�n de reflexi�n desde punto;
    //            color_r = traza_RR(rayo_r, profundidad + 1);
    //            escalar color_r por el coeficiente especular y a�adir a color;
    //        }
    //        if (objeto es transparente) {
    //            if (no ocurre la reflexi�n interna total) {
    //                rayo_t = rayo en la direcci�n de refracci�n desde punto;
    //                color_t = traza_RR(rayo_t, profundidad + 1);
    //                escalar color_t por el coeficiente de transmisi�n y
    //                    a�adir a color;
    //            }
    //        }
    //    }

    //    return color; /* Devolver color del rayo. */
    

    FreeImage_Save(FIF_PNG, bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
}