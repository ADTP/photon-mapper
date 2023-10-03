#include "../SDL2-2.0.12/include/SDL.h"
#include "../Freeimage/FreeImage.h"

#include <stdio.h>
#include <conio.h>
#include <time.h>       /* time */
#include <direct.h>
#include <tchar.h>

#include "Escena.h"
#include "Pantalla.h"
#include "Camara.h"
#include "Foton.h"

#include <vector>
#include <iostream>
#include <string>
#include <random> 
#include <functional> 
#include <list> 

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "nanoflann/nanoflann.h"
#include "nanoflann/utils.h"

#include <embree3/rtcore.h>
#include <limits>
#include <iostream>

using namespace std;

mt19937 engine;  //Mersenne twister MT19937

uniform_real_distribution<double> distribution1(0.0, 1.0);
static auto generator1 = std::bind(distribution1, engine);

uniform_real_distribution<double> distribution2(-1.0, 1.0);
static auto generator2 = std::bind(distribution2, engine);

void trazarFoton(RGBQUAD color, vec3 origen, vec3 direccion, PointCloud &listaFotones, int profundidad, float indiceRefraccionActual) {
    //Rayo* trazaFoton = new Rayo(origen, direccion, indiceRefraccionActual);
    //Escena* escena = Escena::getInstance();

    //int indiceMasCerca = -1;
    //float distancia = 100000;

    //vec3 interseccionMasCercana;

    //for (int i = 0; i < escena->elementos.size(); i++) {
    //    float t = escena->elementos[i]->interseccionRayo(trazaFoton);

    //    if (t > 1e-3) {
    //        vec3 interseccion = trazaFoton->origen + trazaFoton->direccion * t;
    //        float distanciaNueva = distance(trazaFoton->origen, interseccion);

    //        if (distanciaNueva < distancia) {
    //            distancia = distanciaNueva;
    //            indiceMasCerca = i;
    //            interseccionMasCercana = interseccion;
    //        }
    //    }
    //}

    //// si hay objeto intersectado
    //if (indiceMasCerca != -1) {
    //    Elemento* elementoIntersectado = escena->elementos[indiceMasCerca];
    //    vec3 normalInterseccion = elementoIntersectado->normalDelPunto(interseccionMasCercana);
    //    vec3 direccionIncidente = direccion - origen;

    //    vec3 reflexionEspecular = elementoIntersectado->getCoeficienteReflexionEspecular();
    //    vec3 reflexionDifusa = elementoIntersectado->getCoeficienteReflexionDifusa();

    //    double a = generator1();

    //    double dividendoDifusa = max({reflexionDifusa.r * color.rgbRed, reflexionDifusa.g * color.rgbGreen, reflexionDifusa.b * color.rgbBlue});
    //    double divisorDifusa = max({color.rgbRed, color.rgbGreen, color.rgbBlue});
    //    double factorPotenciaDifusa = dividendoDifusa / divisorDifusa; // Pd

    //    double dividendoEspecular = max({ reflexionEspecular.r * color.rgbRed, reflexionEspecular.g * color.rgbGreen, reflexionEspecular.b * color.rgbBlue });
    //    double divisorEspecular = max({ color.rgbRed, color.rgbGreen, color.rgbBlue });
    //    double factorPotenciaEspecular = dividendoEspecular / divisorEspecular; // Ps

    //    /*cout << "a: " << a << "\n";
    //    cout << "Pd: " << factorPotenciaDifusa << "\n";
    //    cout << "Pd: " << factorPotenciaEspecular << "\n\n";*/

    //    vec3 direccionReflejada{};
    //    if (a < factorPotenciaDifusa) {
    //        RGBQUAD potenciaReflejada = { color.rgbRed * reflexionDifusa.r / factorPotenciaDifusa, color.rgbGreen * reflexionDifusa.g / factorPotenciaDifusa, color.rgbBlue * reflexionDifusa.b / factorPotenciaDifusa };
    //        potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };
    //        if (profundidad > 1) {
    //            listaFotones.pts.push_back(Foton(interseccionMasCercana, color, 0, 0, 0)); // TODO: FALTAN ANGULOS Y FLAG PARA KDTREE
    //        } 
    //        
    //        do {
    //            direccionReflejada = { generator2(), generator2(), generator2() };
    //        } while (dot(normalInterseccion, direccionReflejada) <= 0);
    //        direccionReflejada = interseccionMasCercana - direccionReflejada;
    //        trazarFoton(potenciaReflejada, interseccionMasCercana+0.01f * elementoIntersectado->normalDelPunto(interseccionMasCercana), direccionReflejada, listaFotones, profundidad + 1, trazaFoton->refraccionObjetoActual);

    //    } else if (a < factorPotenciaDifusa + factorPotenciaEspecular) {
    //        RGBQUAD potenciaReflejada = { color.rgbRed * reflexionEspecular.r / factorPotenciaEspecular, color.rgbGreen * reflexionEspecular.g / factorPotenciaEspecular, color.rgbBlue * reflexionEspecular.b / factorPotenciaEspecular };
    //        potenciaReflejada = { potenciaReflejada.rgbRed, potenciaReflejada.rgbGreen, potenciaReflejada.rgbBlue };

    //        float refraccionProximoRayo = elementoIntersectado->getRefraccion();
    //        if (elementoIntersectado->getReflexion() > 0.0) { // si hay que reflejar
    //            direccionReflejada = reflect(direccionIncidente, normalInterseccion);

    //        } else if (elementoIntersectado->getRefraccion() > 0.0) { // si hay que refractar // HABRIA QUE SACAR ESTO Y DEJARLO SOLO PARA EL MAPA DE CAUSTICAS?
    //            if (dot(normalInterseccion, direccionIncidente) > 0) {
    //                refraccionProximoRayo = 1.00029;
    //            }

    //            direccionReflejada = refract(direccionIncidente, normalInterseccion, trazaFoton->refraccionObjetoActual / refraccionProximoRayo);
    //        } 

    //        direccionReflejada = interseccionMasCercana - direccionReflejada;
    //        trazarFoton(potenciaReflejada, interseccionMasCercana, direccionReflejada, listaFotones, profundidad + 1, refraccionProximoRayo);

    //    } else {
    //        if (elementoIntersectado->getDifusa() > 0.0) {
    //            listaFotones.pts.push_back(Foton(interseccionMasCercana, color, 0, 0, 0));
    //        }
    //    }
    //}
}

void generarMapaDeFotones(PointCloud &listaFotones) {
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

int cargarObjeto(vector<float> &vertices) {
    std::string inputfile = "Modelos\\plano2.obj";
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

                vertices.push_back(vx);
                vertices.push_back(vy -1);
                vertices.push_back(vz + 5);
               
            }

            index_offset += fv;

            // per-face material
            //shapes[s].mesh.material_ids[f];
        }
    }
    return shapes[0].mesh.num_face_vertices.size() * 3;
}

int _tmain(int argc, _TCHAR* argv[])
{
    FreeImage_Initialise();

    Escena* escena = Escena::getInstance();
    Camara* camara = Camara::getInstance();

    Pantalla* pantalla = Pantalla::getInstance();
    pantalla->cargarMalla(camara);

    RTCDevice device = rtcNewDevice(NULL);
    RTCScene scene = rtcNewScene(device);

    //Whitted* whitted = new Whitted();

    // AGREGAR MAPA DE PROYECCIONES
    //cargarObjeto(escena);

    /*cout << "Empieza Foton Map ";
    vector<Foton> listaFotones = {};
    generarMapaDeFotones(listaFotones);
    kdt::KDTree<Foton> mapa(listaFotones);
    cout << "Termina Foton Map ";*/

    //PointCloud listaFotones;
    //if (escena->generarMapas) {
    //    generarMapaDeFotones(listaFotones);

    //    std::ofstream outfile("Mapas\\foton_list.dat", std::ios::binary);
    //    for (const Foton& foton : listaFotones.pts) {
    //        foton.serializar(outfile);
    //    }
    //    outfile.close();

    //} else {
    //    std::ifstream infile("Mapas\\foton_list.dat", std::ios::binary);
    //    listaFotones.pts.clear();
    //    while (!infile.eof()) {
    //        Foton foton;
    //        foton.deserializar(infile);
    //        if (!infile.eof()) {
    //            listaFotones.pts.push_back(foton);
    //        }
    //    }
    //    infile.close();

    //}

    //using CustomKDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3 /* dim */>;
    //CustomKDTree index(3 /*dim*/, listaFotones, { 10 /* max leaf */ }); // PROBAR CAMBIANDO LA CANTIDAD MAXIMA DE HOJAS

    


    //float* vb = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), 3);
    //vb[0] = 0.f; vb[1] = 0.f; vb[2] = -1.f; // 1st vertex
    //vb[3] = 1.f; vb[4] = 0.f; vb[5] = -1.f; // 2nd vertex
    //vb[6] = 0.f; vb[7] = 1.f; vb[8] = -1.f; // 3rd vertex

    //unsigned* ib = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), 1);
    //ib[0] = 0; ib[1] = 1; ib[2] = 2;

    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    vector<float> vertices = {};
    int caras = cargarObjeto(vertices);

    // set vertices
    float* vb = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), vertices.size());
    for (int i = 0; i < vertices.size(); ++i) { vb[i] = vertices[i]; }
    
    // set indices
    size_t nCaras = caras;
    unsigned* ib = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), nCaras * 3);
    for (int i = 0; i < caras - 3; ++i) { ib[i] = i; }

    rtcCommitGeometry(geom);
    rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(scene);

    //RTCRayHit rayhit;
    //rayhit.ray.org_x = 0.f; rayhit.ray.org_y = 0.f; rayhit.ray.org_z = -1.f;
    //rayhit.ray.dir_x = 0.f; rayhit.ray.dir_y = 0.f; rayhit.ray.dir_z = 1.f;
    //rayhit.ray.tnear = 0.f;
    //rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    //rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    //rayhit.ray.flags = 0;

    //RTCIntersectContext context;
    //rtcInitIntersectContext(&context);

    //rtcIntersect1(scene, &context, &rayhit);

    //if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    //    std::cout << "Intersection at t = " << rayhit.ray.tfar << std::endl;
    //}
    //else {
    //    std::cout << "No Intersection" << std::endl;
    //}

    RGBQUAD blanco = { 255, 255, 255 };
    RGBQUAD rojo = { 255, 0, 0 };
    rojo = { rojo.rgbRed, rojo.rgbGreen, rojo.rgbBlue };

    RGBQUAD negro = { 0, 0, 0 };
    for (int y = 0; y < pantalla->altura; y++) {
        for (int x = 0; x < pantalla->ancho; x++) {
            //Rayo *rayo = new Rayo(camara->posicion, pantalla->pixelesPantalla[x][y], 1.00029f, x, y);
            RTCRayHit rayhit;
            rayhit.ray.org_x = camara->posicion.x;
            rayhit.ray.org_y = camara->posicion.y;
            rayhit.ray.org_z = camara->posicion.z;
            rayhit.ray.dir_x = pantalla->pixelesPantalla[x][y].x - camara->posicion.x;
            rayhit.ray.dir_y = pantalla->pixelesPantalla[x][y].y - camara->posicion.y;
            rayhit.ray.dir_z = pantalla->pixelesPantalla[x][y].z - camara->posicion.z;
            rayhit.ray.tnear = 0.f;
            rayhit.ray.tfar = std::numeric_limits<float>::infinity();
            rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

            RTCIntersectContext context;
            rtcInitIntersectContext(&context);
            rtcIntersect1(scene, &context, &rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                FreeImage_SetPixelColor(pantalla->bitmap, x, y, &rojo);
            }
            else {
                FreeImage_SetPixelColor(pantalla->bitmap, x, y, &negro);
            }

            /*RGBQUAD color = whitted->traza_RR(rayo, 0);
            FreeImage_SetPixelColor(pantalla->bitmap, x, y, &color);*/

            //vec3 interseccionMasCercana;
            //float distancia = 100000;
            //int indiceMasCerca = -1;

            //for (int i = 0; i < escena->elementos.size(); i++) {
            //    float t = escena->elementos[i]->interseccionRayo(rayo);

            //    if (t > 1e-3) {
            //        vec3 interseccion = rayo->origen + rayo->direccion * t;
            //        float distanciaNueva = distance(rayo->origen, interseccion);

            //        if (distanciaNueva < distancia) {
            //            distancia = distanciaNueva;
            //            indiceMasCerca = i;
            //            interseccionMasCercana = interseccion;
            //        }
            //    }
            //}

            //if (indiceMasCerca != -1) {

            //    const float query[3] = { interseccionMasCercana.x, interseccionMasCercana.y, interseccionMasCercana.z };
            //    const float radius = 0.001;
            //    std::vector <nanoflann::ResultItem<uint32_t, float>> matches;
            //    nanoflann::SearchParameters params;
            //    params.sorted = true;

            //    index.radiusSearch(&query[0], radius, matches, params); // Devuelve en matches un vector de pares con el siguiente formato: (indice, distancia)
            //    if (matches.size() > 0) {
            //        Foton masCercano = listaFotones.pts[matches[0].first];
            //        FreeImage_SetPixelColor(pantalla->bitmap, x, y, &masCercano.potencia);
            //    }

                /*Foton interseccion = Foton(interseccionMasCercana, { 0,0,0 }, 0, 0, 0);
                vector<int> indexes = mapa.radiusSearch(interseccion, 0.01);

                if (indexes.size() > 0) {
                    Foton masCercano = listaFotones[indexes.front()];
                    FreeImage_SetPixelColor(pantalla->bitmap, x, y, &masCercano.potencia);
                }*/
            //}
        }
 
    }
    //rtcReleaseScene(scene);
    //rtcReleaseDevice(device);
    FreeImage_Save(FIF_PNG, pantalla->bitmap, "Final\\AA Resultado.png", 0);

    /*FreeImage_Save(FIF_PNG, pantalla->bitmapAmbiente,"Final\\Ambiente.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapDifuso, "Final\\Difuso.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapEspecular, "Final\\Especular.png", 0);

    FreeImage_Save(FIF_PNG, pantalla->bitmapReflexion, "Final\\Reflexion.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapTransmision, "Final\\Transmision.png", 0);*/

    FreeImage_DeInitialise();

    return 0;
}
