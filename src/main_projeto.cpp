#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "mesh_loader.hpp"
#include "material.hpp"
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

    // -- -NOVO : CRIANDO MATERIAIS DISTINTOS-- -
        // (Ambiente, Difusa, Especular, Brilho)
    auto material_chao = make_shared<material>(color(0.1, 0.1, 0.1), color(0.2, 0.8, 0.2), color(0.1, 0.1, 0.1), 10.0);  // Verde fosco
    auto material_azul = make_shared<material>(color(0.1, 0.1, 0.1), color(0.2, 0.5, 0.8), color(0.8, 0.8, 0.8), 64.0);      // Azul brilhante
    auto material_vermelho = make_shared<material>(color(0.1, 0.0, 0.0), color(0.8, 0.1, 0.1), color(0.9, 0.9, 0.9), 128.0); // Vermelho plástico
    auto material_ouro = make_shared<material>(color(0.2, 0.1, 0.0), color(0.8, 0.6, 0.2), color(1.0, 0.9, 0.5), 256.0);     // Dourado metálico

    // --- APLICANDO MATERIAIS NAS ESFERAS ---
    // Passando o material como o último parâmetro
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_chao)); // chão
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, material_azul));
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.2, material_vermelho));
    world.add(make_shared<sphere>(point3(0, 0, -6), 2, material_ouro));
    // --- CONSTRUINDO UMA PIRÂMIDE MANUALMENTE ---

    auto material_modelo = make_shared<material>(
        color(0.1, 0.1, 0.1), // Ambiente
        color(0.5, 0.5, 0.5), // Difuso (cinza)
        color(1.0, 1.0, 1.0), // Especular (brilho branco)
        32.0                  // Shininess
    );

    // 1. Definindo os vértices no espaço 3D
    // Vamos colocar a pirâmide afastada da câmera no eixo Z, e com a base no eixo Y = 0
    point3 topo(0.0, 1.5, -2.0);

    point3 base_frente_esq(-1.0, 0.0, -1.0);
    point3 base_frente_dir(1.0, 0.0, -1.0);
    point3 base_tras_dir(1.0, 0.0, -3.0);
    point3 base_tras_esq(-1.0, 0.0, -3.0);

    // 2. Montando as paredes laterais (4 triângulos)
    // Nota: A ordem dos pontos (sentido anti-horário) dita para onde a Normal vai apontar.
    world.add(make_shared<triangle>(base_frente_dir, base_frente_esq, topo, material_modelo)); // Frente
    world.add(make_shared<triangle>(base_tras_dir, base_frente_dir, topo, material_modelo));   // Direita
    world.add(make_shared<triangle>(base_tras_esq, base_tras_dir, topo, material_modelo));     // Trás
    world.add(make_shared<triangle>(base_frente_esq, base_tras_esq, topo, material_modelo));   // Esquerda

    // 3. Montando a base quadrada (2 triângulos)
    world.add(make_shared<triangle>(base_frente_esq, base_frente_dir, base_tras_dir, material_modelo));
    world.add(make_shared<triangle>(base_frente_esq, base_tras_dir, base_tras_esq, material_modelo));

    // cena 
    /*world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));      // centro (i, j, k), raio
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // esfera verde grande, chão
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.2));
    world.add(make_shared<sphere>(point3(0, 0, -6), 2));
    world.add(make_shared<sphere>(point3(-2, 0, -0.78), 0.4));
    world.add(make_shared<sphere>(point3(2, 0, -1.3), 0.6));
    world.add(make_shared<sphere>(point3(3, 0, -2), 0.8));*/

        // 2. Passe esse material para o carregador de malha
        load_obj("meu_modelo.obj", world, material_modelo, vec3(0, 0, -2), 1.0);

        //// world.add(make_shared<sphere>(point3(-1, 0, -3), 0.75));
        // world.add(make_shared<sphere>(point3(1, 6, -1), 0.55));
        //  world.add(make_shared<sphere>(point3(2, 3, 2), 0.35));
        //  world.add(make_shared<sphere>(point3(-1, 0, 1), 0.8));
        //  world.add(make_shared<sphere>(point3(0, 100, 3), 10));

        // demora demais para gerar o bule mas funciona
        // 1. Defina um material para o bule (ex: metálico)
       
       /* auto material_bule = make_shared<material>(
            color(0.1, 0.1, 0.1), // Ambiente
            color(0.5, 0.5, 0.5), // Difuso
            color(1.0, 1.0, 1.0), // Especular
            64.0                  // Shininess
        );

        // 2. Carregue o arquivo
        // Ajuste a posição (vec3) e a escala (double) conforme necessário
        load_obj("teapot.obj", world, material_bule, vec3(0, 0, -5), 1.0); */

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