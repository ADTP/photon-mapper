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
#include "Whitted.h"

using namespace std;

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

    Whitted* whitted = new Whitted();

    // Whitted - recorrido de la maya tirando rayos desde la camara.
    for (int i = pantalla->altura - 1; i >= 0; i--) {
        for (int j = 0; j < pantalla->ancho; j++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[i][j], 1.00029f);
            RGBQUAD color = whitted->traza_RR(rayo, 0);
            FreeImage_SetPixelColor(pantalla->bitmap, j, i, &color);
        }
    }

    FreeImage_Save(FIF_PNG, pantalla->bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
}
