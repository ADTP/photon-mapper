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
#include "Triangulo.h"

#include <vector>
#include <iostream>
#include <string>
#include <random> 
#include <functional> 
#include <list> 

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

mt19937 engine;  //Mersenne twister MT19937

uniform_real_distribution<double> distribution1(0.0, 1.0);
static auto generator1 = std::bind(distribution1, engine);

uniform_real_distribution<double> distribution2(-1.0, 1.0);
static auto generator2 = std::bind(distribution2, engine);

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

        vec3 reflexionEspecular = elementoIntersectado->getCoeficienteReflexionEspecular();
        vec3 reflexionDifusa = elementoIntersectado->getCoeficienteReflexionDifusa();

        double a = generator1();

        double dividendoDifusa = max({reflexionDifusa.r * color.rgbRed, reflexionDifusa.g * color.rgbGreen, reflexionDifusa.b * color.rgbBlue});
        double divisorDifusa = max({color.rgbRed, color.rgbGreen, color.rgbBlue});
        double factorPotenciaDifusa = dividendoDifusa / divisorDifusa; // Pd

        double dividendoEspecular = max({ reflexionEspecular.r * color.rgbRed, reflexionEspecular.g * color.rgbGreen, reflexionEspecular.b * color.rgbBlue });
        double divisorEspecular = max({ color.rgbRed, color.rgbGreen, color.rgbBlue });
        double factorPotenciaEspecular = dividendoEspecular / divisorEspecular; // Ps

        /*cout << "a: " << a << "\n";
        cout << "Pd: " << factorPotenciaDifusa << "\n";
        cout << "Pd: " << factorPotenciaEspecular << "\n\n";*/

        vec3 direccionReflejada{};
        if (a < factorPotenciaDifusa) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.r / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.g / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.b / factorPotenciaDifusa };
            potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
            if (profundidad > 1) {
                listaFotones.push_back(Foton(interseccionMasCercana, color, 0, 0, 0)); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
            } 
            
            do {
                direccionReflejada = { generator2(), generator2(), generator2() };
            } while (dot(normalInterseccion, direccionReflejada) <= 0);
            direccionReflejada = interseccionMasCercana - direccionReflejada;
            trazarFoton(potenciaReflejada, interseccionMasCercana+0.01f * elementoIntersectado->normalDelPunto(interseccionMasCercana), direccionReflejada, listaFotones, profundidad + 1, trazaFoton->refraccionObjetoActual);

        } else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
            RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.r / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.g / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.b / factorPotenciaEspecular };
            potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };

            float refraccionProximoRayo = elementoIntersectado->getRefraccion();
            if (elementoIntersectado->getReflexion() > 0.0) { // si hay que reflejar
                direccionReflejada = reflect(direccionIncidente, normalInterseccion);

            } else if (elementoIntersectado->getRefraccion() > 0.0) { // si hay que refractar // HABRIA QUE SACAR ESTO Y DEJARLO SOLO PARA EL MAPA DE CAUSTICAS?
                if (dot(normalInterseccion, direccionIncidente) > 0) {
                    refraccionProximoRayo = 1.00029;
                }

                direccionReflejada = refract(direccionIncidente, normalInterseccion, trazaFoton->refraccionObjetoActual / refraccionProximoRayo);
            } 

            direccionReflejada = interseccionMasCercana - direccionReflejada;
            trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, profundidad + 1, refraccionProximoRayo);

        } else {
            if (elementoIntersectado->getDifusa() > 0.0) {
                listaFotones.push_back(Foton(interseccionMasCercana, color, 0, 0, 0));
            }
        }
    }
}

void generarMapaDeFotones(vector<Foton> &listaFotones) {
    Escena* escena = Escena::getInstance();

    vec3 dirFoton = { 0, 0, 0 };

    // Calcular la potencia total para poder ver cuantos fotones aporta cada luz a la escena
    int potenciaTotal = 0;
    for (int i = 0; i < escena->luces.size(); i++) {
        potenciaTotal += escena->luces[i]->watts;
    }

    // Iterar sobre todas las luces de la escena emitiendo fotones
    for (int i = 0; i < escena->luces.size(); i++) {
        int fotonesAEmitir = escena->luces[i]->watts * escena->cantidadDeFotones / potenciaTotal;
        int fotonesEmitidos = 0;

        //cout << "Fotones emitidos: " << fotonesEmitidos << "\n\n";
        while (fotonesEmitidos < fotonesAEmitir) {
            do {
                dirFoton.x = generator2();
                dirFoton.y = generator2();
                dirFoton.z = generator2();
            } while (pow(dirFoton.x,2) + pow(dirFoton.y, 2) + pow(dirFoton.z, 2) > 1);
            dirFoton = dirFoton + escena->luces[i]->posicion;
            trazarFoton(escena->luces[i]->color, escena->luces[i]->posicion, dirFoton, listaFotones, 0, 1.00029);
            fotonesEmitidos++;
            // QUESTION: SCALE POWER OF STORED PHOTONS ??

            /*if (fotonesEmitidos % 100 == 0) {
                cout << "Fotones Emitidos: " << fotonesEmitidos << "\n";
            }*/
        }
    }
}

void cargarObjeto(Escena* escena) {
    std::string inputfile = "Modelos\\sphere.obj";
    tinyobj::ObjReaderConfig reader_config;
    //reader_config.mtl_search_path = "./"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        cout << shapes[s].mesh.num_face_vertices.size() / 3;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            vec3 puntos[3] = {};
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                // Check if `normal_index` is zero or positive. negative = no normal data
                //if (idx.normal_index >= 0) {
                //    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                //    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                //    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                //}

                //// Check if `texcoord_index` is zero or positive. negative = no texcoord data
                //if (idx.texcoord_index >= 0) {
                //    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                //    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                //}

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                puntos[v] = { vx, vy, vz };
               
            }
            vec3 traslacion = { 3, 1, -3 };
            escena->elementos.push_back(new Triangulo(
                puntos[0] + traslacion, puntos[1] + traslacion, puntos[2] + traslacion,
                { 255, 255, 255 }, 0.5, 0.5, 0, 0, 0, 0, { 0, 0, 0.3 }, { 0, 0, 0 }
            ));

            index_offset += fv;

            // per-face material
            //shapes[s].mesh.material_ids[f];
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

    //Whitted* whitted = new Whitted();

    // AGREGAR MAPA DE PROYECCIONES
    cargarObjeto(escena);

    cout << "Empieza Foton Map ";
    vector<Foton> listaFotones = {};
    generarMapaDeFotones(listaFotones);
    kdt::KDTree<Foton> mapa(listaFotones);
    cout << "Termina Foton Map ";

    for (int y = 0; y < pantalla->altura; y++) {
        for (int x = 0; x < pantalla->ancho; x++) {
            Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[x][y], 1.00029f, x, y);

            /*RGBQUAD color = whitted->traza_RR(rayo, 0);
            FreeImage_SetPixelColor(pantalla->bitmap, x, y, &color);*/

            vec3 interseccionMasCercana;
            float distancia = 100000;
            int indiceMasCerca = -1;

            for (int i = 0; i < escena->elementos.size(); i++) {
                float t = escena->elementos[i]->interseccionRayo(rayo);

                if (t > 1e-3) {
                    vec3 interseccion = rayo->origen + rayo->direccion * t;
                    float distanciaNueva = distance(rayo->origen, interseccion);

                    if (distanciaNueva < distancia) {
                        distancia = distanciaNueva;
                        indiceMasCerca = i;
                        interseccionMasCercana = interseccion;
                    }
                }
            }

            if (indiceMasCerca != -1) {

                Foton interseccion = Foton(interseccionMasCercana, { 0,0,0 }, 0, 0, 0);
                vector<int> indexes = mapa.radiusSearch(interseccion, 0.01);

                if (indexes.size() > 0) {
                    Foton masCercano = listaFotones[indexes.front()];
                    FreeImage_SetPixelColor(pantalla->bitmap, x, y, &masCercano.potencia);
                }
            }
        }
    }

    FreeImage_Save(FIF_PNG, pantalla->bitmap, "Final\\AA Resultado.png", 0);

    /*FreeImage_Save(FIF_PNG, pantalla->bitmapAmbiente,"Final\\Ambiente.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapDifuso, "Final\\Difuso.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapEspecular, "Final\\Especular.png", 0);

    FreeImage_Save(FIF_PNG, pantalla->bitmapReflexion, "Final\\Reflexion.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapTransmision, "Final\\Transmision.png", 0);*/

    FreeImage_DeInitialise();

    return 0;
}
