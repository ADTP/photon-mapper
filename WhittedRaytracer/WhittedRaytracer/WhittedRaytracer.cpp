#include "../SDL2-2.0.12/include/SDL.h"
#include "../Freeimage/FreeImage.h"

#include <stdio.h>
#include <conio.h>
#include <time.h>       /* time */
#include <direct.h>
#include <tchar.h>

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

    strftime(buffer, sizeof(buffer), "Historial\\Resultado %d-%m-%Y %H:%M:%S", &timeinfo);
    string str(buffer);
    
    const int n = str.length();
    char char_array[40];
    strcpy_s(char_array, str.c_str());
    
    /*int dir = _mkdir(char_array);

    if (dir != 0) {
        return 1;
    }*/

    RGBQUAD color;
    color.rgbRed = 255;
    color.rgbGreen = 234;
    color.rgbBlue = 123;

    for (int i = 0; i < 640; i++) {
        for (int j = 0; j < 480; j++) {
            FreeImage_SetPixelColor(bitmap, i, j, &color);
        }
    }

    FreeImage_Save(FIF_PNG, bitmap, char_array, 0);
    FreeImage_DeInitialise();

    return 0;
}