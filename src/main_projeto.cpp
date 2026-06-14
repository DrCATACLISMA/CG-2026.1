#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "mesh_loader.hpp"
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

    // --- CONSTRUINDO UMA PIRÂMIDE MANUALMENTE ---

        // 1. Definindo os vértices no espaço 3D
        // Vamos colocar a pirâmide afastada da câmera no eixo Z, e com a base no eixo Y = 0
        point3 topo(0.0, 1.5, -2.0);

        point3 base_frente_esq(-1.0, 0.0, -1.0);
        point3 base_frente_dir( 1.0, 0.0, -1.0);
        point3 base_tras_dir( 1.0, 0.0, -3.0);
        point3 base_tras_esq(-1.0, 0.0, -3.0);

        // 2. Montando as paredes laterais (4 triângulos)
        // Nota: A ordem dos pontos (sentido anti-horário) dita para onde a Normal vai apontar.
        world.add(make_shared<triangle>(base_frente_dir, base_frente_esq, topo)); // Frente
        world.add(make_shared<triangle>(base_tras_dir, base_frente_dir, topo));   // Direita
        world.add(make_shared<triangle>(base_tras_esq, base_tras_dir, topo));     // Trás
        world.add(make_shared<triangle>(base_frente_esq, base_tras_esq, topo));   // Esquerda

        // 3. Montando a base quadrada (2 triângulos)
        world.add(make_shared<triangle>(base_frente_esq, base_frente_dir, base_tras_dir));
        world.add(make_shared<triangle>(base_frente_esq, base_tras_dir, base_tras_esq));

    // cena 
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));      // centro (i, j, k), raio
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // esfera verde grande, chão
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.2));
    world.add(make_shared<sphere>(point3(0, 0, -6), 2));
    world.add(make_shared<sphere>(point3(-2, 0, -0.78), 0.4));
    world.add(make_shared<sphere>(point3(2, 0, -1.3), 0.6));
    world.add(make_shared<sphere>(point3(3, 0, -2), 0.8));

    load_obj("meu_modelo.obj", world, vec3(0, 0, -2), 1.0);

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