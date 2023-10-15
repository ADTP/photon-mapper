#include "../SDL2-2.0.12/include/SDL.h"
#include "../Freeimage/FreeImage.h"

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <direct.h>
#include <tchar.h>

#include "Escena.h"
#include "Esfera.h"
#include "Pantalla.h"
#include "Camara.h"
#include "Foton.h"
#include "PhotonMapper.h"
#include "RayTracer.h"
#include "ObjectLoader.h"

#include <vector>
#include <iostream>
#include <string>
#include <random> 
#include <functional> 
#include <list> 
#include <limits>
#include <iostream>

#include "nanoflann/nanoflann.h"
#include "nanoflann/utils.h"

#include <embree3/rtcore.h>
#include <embree3/rtcore_geometry.h>

using namespace std;
//
//void sphereBoundsFunction(const struct RTCBoundsFunctionArguments* args)
//{
//    Esfera* esfera = (Esfera*)args->geometryUserPtr;
//    vec3 posicion = esfera->posicion;
//    float radio = esfera->radio;
//    RTCBounds bounds_o = *args->bounds_o;
//    bounds_o.lower_x = posicion.x - radio;
//    bounds_o.lower_y = posicion.y - radio;
//    bounds_o.lower_z = posicion.z - radio;
//    bounds_o.upper_x = posicion.x + radio;
//    bounds_o.upper_y = posicion.y + radio;
//    bounds_o.upper_z = posicion.z + radio;
//}
//
//void sphereIntersectionFunction(const struct RTCIntersectFunctionNArguments* args) {
//    Esfera* esfera = (Esfera*)args->geometryUserPtr;
//    RTCRayHitN* rayo = (RTCRayHitN*)args->rayhit;
//    RTCRay* rayo2 = (RTCRay*)rayo;
//    RTCHit* rayo3 = (RTCHit*)rayo;
//    vec3 origenRayo = { rayo2->org_x, rayo2->org_y, rayo2->org_z };
//    vec3 direccionRayo = { rayo2->dir_x, rayo2->dir_y, rayo2->dir_z };
//    float A = dot(direccionRayo, direccionRayo);
//    float B = 2 * dot(origenRayo - esfera->posicion, direccionRayo);
//    float C = dot(origenRayo - esfera->posicion, origenRayo - esfera->posicion) - (esfera->radio * esfera->radio);
//    
//    float raizCuadrada = B * B - 4 * A * C;
//    if (raizCuadrada > 0) {
//        float t1 = (-B - sqrt(raizCuadrada)) / (2 * A);
//        float t2 = (-B + sqrt(raizCuadrada)) / (2 * A);
//        float masCercano = (t1 < t2) ? t1 : t2;
//        rayo2->tfar = masCercano;
//        rayo3->primID = args->primID;
//    }
//
//}

int _tmain(int argc, _TCHAR* argv[])
{
    FreeImage_Initialise();

    Escena* escena = Escena::getInstance();
    Camara* camara = Camara::getInstance();

    PhotonMapper photonMapper;
    RayTracer rayTracer;
    ObjectLoader objectLoader;

    Pantalla* pantalla = Pantalla::getInstance();
    pantalla->cargarMalla(camara);


    cout << "Cargado de objetos a la escena de Embree \n\n";
    RTCDevice device = rtcNewDevice(NULL);
    RTCScene scene = rtcNewScene(device);
    for (Elemento* objeto : escena->elementos) {
        RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        vector<float> vertices = {};
        int caras = objectLoader.cargarObjeto(vertices, objeto);

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

    //RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
    //rtcSetGeometryUserPrimitiveCount(geometry, numPrimitives);
    //rtcSetGeometryUserData(geometry, userGeometryRepresentation);
    //rtcSetGeometryBoundsFunction(geometry, boundsFunction);
    //rtcSetGeometryIntersectFunction(geometry, intersectFunction);
    //rtcSetGeometryOccludedFunction(geometry, occludedFunction);

 /*   vec3 pos = { 0,0,0 };
    Esfera esfera(pos, 1);

    RTCGeometry sphere = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
    rtcSetGeometryUserPrimitiveCount(sphere, 1);
    rtcSetGeometryUserData(sphere, &esfera);
    rtcSetGeometryBoundsFunction(sphere, (RTCBoundsFunction)sphereBoundsFunction, &esfera);
    rtcCommitScene(scene);*/
    rtcCommitScene(scene);

    cout << "Generacion de mapas de fotones \n\n";
    PointCloud mapaGlobal, mapaCausticas;
    photonMapper.generacionDeMapas(escena, mapaGlobal, mapaCausticas, scene);


    cout << "Generacion de KDTs a partir de los mapas \n\n";
    using CustomKDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 3>;
    CustomKDTree indexGlobal(3 /*dim*/, mapaGlobal, { 10 /* max leaf */ }); // PROBAR CAMBIANDO LA CANTIDAD MAXIMA DE HOJAS
    CustomKDTree indexCausticas(3 /*dim*/, mapaCausticas, { 10 /* max leaf */ }); // PROBAR CAMBIANDO LA CANTIDAD MAXIMA DE HOJAS


    cout << "Calculos de radiancia y generacion de imagenes \n\n";
    for (int y = 0; y < pantalla->altura; y++) {
        cout << "HOLA";
        for (int x = 0; x < pantalla->ancho; x++) {
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

            // iluminacion indirecta
            RGBQUAD colorIluminacionDirecta = rayTracer.iluminacionDirecta(scene, rayhit, escena);
            FreeImage_SetPixelColor(pantalla->bitmapDirecta, x, y, &colorIluminacionDirecta);

            // iluminacion indirecta
            /*RGBQUAD colorIluminacionIndirecta = rayTracer.iluminacionIndirecta(scene, rayhit, escena, mapaGlobal, &indexGlobal);
            FreeImage_SetPixelColor(pantalla->bitmapIndirecta, x, y, &colorIluminacionIndirecta);*/
            RGBQUAD colorIluminacionIndirecta = rayTracer.iluminacionIndirecta(scene, rayhit, escena, mapaGlobal, &indexGlobal);
            FreeImage_SetPixelColor(pantalla->bitmapIndirecta, x, y, &colorIluminacionIndirecta);

            // causticas
            RGBQUAD colorIluminacionCausticas = rayTracer.iluminacionCausticas(scene, rayhit, escena, mapaCausticas, &indexCausticas);
            FreeImage_SetPixelColor(pantalla->bitmapCausticas, x, y, &colorIluminacionCausticas);

            // especular
            
            // resultado
            RGBQUAD total;
            total.rgbRed = std::min((int)(colorIluminacionDirecta.rgbRed + colorIluminacionIndirecta.rgbRed + colorIluminacionCausticas.rgbRed), 255);
            total.rgbGreen = std::min((int)(colorIluminacionDirecta.rgbGreen + colorIluminacionIndirecta.rgbGreen + colorIluminacionCausticas.rgbGreen), 255);
            total.rgbBlue = std::min((int)(colorIluminacionDirecta.rgbBlue + colorIluminacionIndirecta.rgbBlue + colorIluminacionCausticas.rgbBlue), 255);
            FreeImage_SetPixelColor(pantalla->bitmapResultado, x, y, &total);
        }
        
        if (y % 50 == 0) {
            cout << y << " / " << pantalla->altura << "\n\n";
        }
    }

    rtcReleaseScene(scene);
    rtcReleaseDevice(device);

    FreeImage_Save(FIF_PNG, pantalla->bitmapDirecta, "Final\\1-Directa.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapIndirecta, "Final\\2-Indirecta.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapCausticas, "Final\\3-Causticas.png", 0);
    FreeImage_Save(FIF_PNG, pantalla->bitmapResultado, "Final\\4-Resultado.png", 0);

    FreeImage_DeInitialise();

    return 0;
}
