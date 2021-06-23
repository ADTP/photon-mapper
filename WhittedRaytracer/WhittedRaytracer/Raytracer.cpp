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

#include <vector>
#include <iostream>
#include <string>

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

int _tmain(int argc, _TCHAR* argv[])
{
    FreeImage_Initialise();

    Escena* escena = Escena::getInstance();
    Camara* camara = Camara::getInstance();

    Pantalla* pantalla = Pantalla::getInstance();
    pantalla->cargarMalla(camara);

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
