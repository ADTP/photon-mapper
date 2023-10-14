#include "../SDL2-2.0.12/include/SDL.h"
#include "../Freeimage/FreeImage.h"

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <direct.h>
#include <tchar.h>

#include "Escena.h"
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

using namespace std;

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

            // iluminacion directa
            /*RGBQUAD colorIluminacionDirecta = rayTracer.iluminacionDirecta(scene, rayhit, escena);
            FreeImage_SetPixelColor(pantalla->bitmapDirecta, x, y, &colorIluminacionDirecta);*/

            // iluminacion indirecta
            /*RGBQUAD colorIluminacionIndirecta = rayTracer.iluminacionIndirecta(scene, rayhit, escena, mapaGlobal, &indexGlobal);
            FreeImage_SetPixelColor(pantalla->bitmapIndirecta, x, y, &colorIluminacionIndirecta);*/

            // causticas
            RGBQUAD colorIluminacionCausticas = rayTracer.iluminacionCausticas(scene, rayhit, escena, mapaCausticas, &indexCausticas);
            FreeImage_SetPixelColor(pantalla->bitmapCausticas, x, y, &colorIluminacionCausticas);

            // especular
            
            // resultado
            RGBQUAD total;
            /*total.rgbRed = std::min((int)(colorIluminacionDirecta.rgbRed + colorIluminacionIndirecta.rgbRed), 255);
            total.rgbGreen = std::min((int)(colorIluminacionDirecta.rgbGreen + colorIluminacionIndirecta.rgbGreen), 255);
            total.rgbBlue = std::min((int)(colorIluminacionDirecta.rgbBlue + colorIluminacionIndirecta.rgbBlue), 255);*/
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
