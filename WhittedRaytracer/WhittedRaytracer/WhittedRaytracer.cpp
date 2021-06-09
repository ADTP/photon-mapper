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
    Camara* camara = new Camara({300, 200, 0}, { 0, 1, 0 }, { 300, 200, 100 },0,0,60,90);





    for (int i = 0; i < 640; i++) {
        for (int j = 0; j < 480; j++) {
            FreeImage_SetPixelColor(bitmap, i, j, &color);
        }
    }

    FreeImage_Save(FIF_PNG, bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
}