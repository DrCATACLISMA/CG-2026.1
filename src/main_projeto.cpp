#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include <fstream>

int main()
{
    // Globais
    hittable_list world;
    camera_parameters cam_1;
    int vec_e[3];
    int vec_u[3];

    // cena
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;

    std::cout << "Digite o campo de visão:\n";
    std::cin >> cam_1.fov;

    std::cout << "Digite a resolução vertical:\n";
    std::cin >> cam_1.ny;

    std::cout << "Digite a resolução horizontal\n";
    std::cin >> cam_1.nx;

    std::cout << "Digite as coordenadas da posição da camera no mundo\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Coordenada n"<< i <<":\n";
        std::cin >> vec_e[i];
    }

    cam_1.eye_e = point3(vec_e[0], vec_e[1], vec_e[2]);

    std::cout << "Ditige o vetor u:\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Coordenada u n"<< i <<":\n";
        std::cin >> vec_u[i];
    }

    cam_1.u = vec3(vec_u[0], vec_u[1], vec_u[2]);

    cam.render(world, cam_1);
}