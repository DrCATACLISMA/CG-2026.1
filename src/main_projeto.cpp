#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include <fstream>

int main()
{

    // 16:9 aspect ratio -> 1920x1080, 1280x720, 854x480, 640x360, 426x240, 256x144
    // 4:3 aspect ratio -> 1600x1200
    // entrada teste 180, 854, 480, 0, 0, 0
    // Globais
    hittable_list world; // lista de objetos
    camera_parameters cam_1;
    int vec_e[3];
    // int vec_u[3];

    // cena 
    // world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));      // centro (i, j, k), raio
    // // world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // esfera verde grande, chão
    // world.add(make_shared<sphere>(point3(-1, 0, -1), 0.2));
    // world.add(make_shared<sphere>(point3(0, 0, -6), 2));
    // world.add(make_shared<sphere>(point3(-2, 0, -0.78), 0.4));
    // world.add(make_shared<sphere>(point3(2, 0, -1.3), 0.6));
    // world.add(make_shared<sphere>(point3(3, 0, -2), 0.8))

    // world.add(make_shared<sphere>(point3(0, 0, 1), 0.5));
    world.add(make_shared<sphere>(point3(0, 0, -0.8), 0.5));
    world.add(make_shared<sphere>(point3(0, 0.6, -0.8), 0.5));
    // world.add(make_shared<sphere>(point3(0, 3, -0.4), 2));

    //// world.add(make_shared<sphere>(point3(-1, 0, -3), 0.75));
    // world.add(make_shared<sphere>(point3(1, 6, -1), 0.55));
    //  world.add(make_shared<sphere>(point3(2, 3, 2), 0.35));
    //  world.add(make_shared<sphere>(point3(-1, 0, 1), 0.8));
    //  world.add(make_shared<sphere>(point3(0, 100, 3), 10));

    camera cam;

    std::cout << "Digite o campo de visão:\n";
    std::cin >> cam_1.fov;

    std::cout << "Digite a resolução horizontal\n";
    std::cin >> cam_1.nx;

    std::cout << "Digite a resolução vertical:\n";
    std::cin >> cam_1.ny;

    cam.aspect_ratio = cam_1.nx / cam_1.ny;
    cam.image_width = cam_1.nx;

    std::cout << "Digite as coordenadas da posição da camera no mundo\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Coordenada n" << i << ":\n";
        std::cin >> vec_e[i];
    }

    cam_1.eye_e = point3(vec_e[0], vec_e[1], vec_e[2]);

    /* std::cout << "Digite o vetor u:\n";
     for (int i = 0; i < 3; i++)
     {
         std::cout << "Coordenada u n" << i << ":\n";
         std::cin >> vec_u[i];
     }

     cam_1.u = vec3(vec_u[0], vec_u[1], vec_u[2]);*/

    cam.render(world, cam_1);
}