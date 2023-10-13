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
#include "PhotonMapper.h"
#include "RayTracer.h"

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

int cargarObjeto(vector<float> &vertices, Elemento* objeto) {
    std::string inputfile = "Modelos\\"+objeto->nombreArchivo;
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

                vertices.push_back(vx + objeto->posicion.x);
                vertices.push_back(vy + objeto->posicion.y);
                vertices.push_back(vz + objeto->posicion.z);
               
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
    
    PhotonMapper photonMapper;
    RayTracer rayTracer;

    Pantalla* pantalla = Pantalla::getInstance();
    pantalla->cargarMalla(camara);

    RTCDevice device = rtcNewDevice(NULL);
    RTCScene scene = rtcNewScene(device);

    for (Elemento* objeto : escena->elementos) {
        RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
        vector<float> vertices = {};
        int caras = cargarObjeto(vertices, objeto);

        // set vertices
        float* vb = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), vertices.size());
        for (int i = 0; i < vertices.size(); ++i) { vb[i] = vertices[i]; }

        // set indices
        size_t nCaras = caras;
        unsigned* ib = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), nCaras * 3);
        for (int i = 0; i < caras; ++i) { ib[i] = i; }

        rtcCommitGeometry(geom);
        rtcAttachGeometry(scene, geom);
        rtcReleaseGeometry(geom);
    }
    rtcCommitScene(scene);

    PointCloud listaFotones;
    if (escena->generarMapas) {
        photonMapper.generarMapaDeFotones(listaFotones, scene);

        // Guardar lista de fotones a binario
        std::ofstream outfile("Mapas\\foton_list.dat", std::ios::binary);
        for (const Foton& foton : listaFotones.pts) {
            foton.serializar(outfile);
        }
        outfile.close();

    } else {
        // Cargar lista de fotones de binario
        std::ifstream infile("Mapas\\foton_list.dat", std::ios::binary);
        listaFotones.pts.clear();
        while (!infile.eof()) {
            Foton foton;
            foton.deserializar(infile);
            if (!infile.eof()) {
                listaFotones.pts.push_back(foton);
            }
        }
        infile.close();
    }

    using CustomKDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3>;
    CustomKDTree index(3 /*dim*/, listaFotones, { 10 /* max leaf */ }); // PROBAR CAMBIANDO LA CANTIDAD MAXIMA DE HOJAS

    RGBQUAD blanco = { 255, 255, 255 };
    RGBQUAD rojo = { 255, 0, 0 };
    rojo = { rojo.rgbRed, rojo.rgbGreen, rojo.rgbBlue };

    RGBQUAD negro = { 0, 0, 0 };
    for (int y = 0; y < pantalla->altura; y++) {
        for (int x = 0; x < pantalla->ancho; x++) {
            /*if (x % 100 == 0) {
                cout << "(" << x << "," << y << ")\n";
            }*/

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

            

            // iluminacion directa
            RGBQUAD colorIluminacionDirecta = rayTracer.iluminacionDirecta(scene, rayhit, escena);
            
            // iluminacion indirecta
            RGBQUAD colorIluminacionIndirecta = rayTracer.iluminacionIndirecta(scene, rayhit, escena, listaFotones, &index);

            // causticas
            RGBQUAD total;
            total.rgbRed = std::min((int)(colorIluminacionDirecta.rgbRed + 2 * colorIluminacionIndirecta.rgbRed), 255);
            total.rgbGreen = std::min((int)(colorIluminacionDirecta.rgbGreen + 2 * colorIluminacionIndirecta.rgbGreen), 255);
            total.rgbBlue = std::min((int)(colorIluminacionDirecta.rgbBlue + 2 * colorIluminacionIndirecta.rgbBlue), 255);

            total.rgbRed = std::min((int)total.rgbRed, 255);
            total.rgbGreen = std::min((int)total.rgbGreen, 255);
            total.rgbBlue = std::min((int)total.rgbBlue, 255);

            FreeImage_SetPixelColor(pantalla->bitmap, x, y, &total);


            //const float query[3] = { interseccionMasCercana.x, interseccionMasCercana.y, interseccionMasCercana.z };
            //const float radius = 0.0001;
            //std::vector <nanoflann::ResultItem<uint32_t, float>> matches;
            //nanoflann::SearchParameters params;
            //params.sorted = true;

            //index.radiusSearch(&query[0], radius, matches, params); // Devuelve en matches un vector de pares con el siguiente formato: (indice, distancia)
            //if (matches.size() > 0) {
            //    Foton masCercano = listaFotones.pts[matches[0].first];
            //    FreeImage_SetPixelColor(pantalla->bitmap, x, y, &masCercano.potencia);
            //}
        }
 
    }

    rtcReleaseScene(scene);
    rtcReleaseDevice(device);

    FreeImage_Save(FIF_PNG, pantalla->bitmap, "Final\\AA Resultado.png", 0);

    /*FreeImage_Save(FIF_PNG, pantalla->bitmapAmbiente,"Final\\Ambiente.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapDifuso, "Final\\Difuso.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapEspecular, "Final\\Especular.png", 0);

    FreeImage_Save(FIF_PNG, pantalla->bitmapReflexion, "Final\\Reflexion.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapTransmision, "Final\\Transmision.png", 0);*/

    FreeImage_DeInitialise();

    return 0;
}
