#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "mesh_loader.hpp"
#include "material.hpp"
#include "triangle.hpp"
#include "node.hpp"
#include "matrix4.hpp"
#include <fstream>
#include <memory>
#include <iomanip>

int main()
{

    // 16:9 aspect ratio -> 1920x1080, 1280x720, 854x480, 640x360, 426x240, 256x144
    // 4:3 aspect ratio -> 1600x1200
    // entrada teste 180, 854, 480, 0, 0, 0
    // posicao vamera 0, 40, 100
    // Globais
    
    hittable_list world;
    camera_parameters cam_1;
    int vec_e[3];

    // material_chao: Sem reflexo (preto 0,0,0)
    auto material_chao = make_shared<material>(color(0.1, 0.1, 0.1), color(0.2, 0.8, 0.2), color(0.1, 0.1, 0.1), color(0, 0, 0), color(0, 0, 0), 10.0, 1.0);

    // material_azul: como água
    auto material_azul = make_shared<material>(
        color(0.0, 0.0, 0.0),
        color(0.0, 0.0, 0.0),
        color(0.5, 0.5, 0.5),
        color(0.3, 0.3, 0.3),
        color(0.4, 0.7, 0.9),
        64.0,
        1.33);

    // material_vermelho: Como vidro vermelho
    auto material_vermelho = make_shared<material>(
        color(0.0, 0.0, 0.0),
        color(0.0, 0.0, 0.0),
        color(0.5, 0.5, 0.5),
        color(0.2, 0.2, 0.2),
        color(0.95, 0.8, 0.8),
        128.0,
        1.5);

    // material_ouro: Altamente reflexivo como um espelho dourado
    auto material_ouro = make_shared<material>(color(0.2, 0.1, 0.0), color(0.8, 0.6, 0.2), color(1.0, 0.9, 0.5), color(0.8, 0.8, 0.5), color(0, 0, 0), 256.0, 1.0);

    // --- 2. OBJETOS FIXOS NO MUNDO ---
    // O chão plano gigante permanece na lista global absoluta
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_chao));

    // --- 3. CONSTRUÇÃO DA HIERARQUIA DE TRANSFORMAÇÕES ---

    // Nó Central (Será uma grande esfera dourada na origem local do grupo)
    auto sistema_articulado = make_shared<node>();
    sistema_articulado->add(make_shared<sphere>(point3(0, 0, 0), 1.0, material_ouro));
    sistema_articulado->scale(vec3(1.3, 1.3, 1.3)); // Escala o "Sol" localmente

    // Nó do Planeta Azul (Filho do sistema central)
    auto planeta_node = make_shared<node>();
    planeta_node->add(make_shared<sphere>(point3(0, 0, 0), 0.5, material_azul));
    planeta_node->translate(vec3(0.5, 0, 0)); // Move o planeta 3 unidades para o lado em relação ao Sol

    // Nó da Lua Vermelha (Filha do Planeta Azul)
    auto lua_node = make_shared<node>();
    lua_node->add(make_shared<sphere>(point3(0, 0, 0), 0.18, material_vermelho));
    lua_node->translate(vec3(0.9, 0.2, 0)); // Move a lua em relação ao planeta

    // Vinculando a árvore hierárquica
    planeta_node->add(lua_node);           // Lua entra no espaço local do planeta
    sistema_articulado->add(planeta_node); // Planeta entra no espaço local do centro

    // --- 4. MOVIMENTAÇÃO DO GRUPO NO ESPAÇO DO MUNDO ---
    // Rotaciona todo o sistema orbital (Sol, Planeta e Lua giram juntos de forma síncrona)
    sistema_articulado->rotate_y(35);

    // Empurra o grupo completo para frente no eixo Z para ficar visível pela câmera
    sistema_articulado->translate(vec3(0, 0, -4.5));

    // ATUALIZAÇÃO MANDATÓRIA: Propaga as matrizes calculadas acumulando com a Identidade
    sistema_articulado->update_transforms(matrix4::identity());

    // Insere o nó completo dentro do mundo renderizável
    world.add(sistema_articulado);

    point3 topo(0.0, 1.5, -2.0);
    point3 base_frente_esq(-1.0, 0.0, -1.0);
    point3 base_frente_dir(1.0, 0.0, -1.0);

    /*world.add(make_shared<triangle>(base_frente_dir, base_frente_esq, topo, material_azul));*/

    // demora demais para gerar o bule mas funciona

    // 2. Carregue o arquivo
    // Ajuste a posição (vec3) e a escala (double) conforme necessário
    /*load_obj("teapot.obj", world, material_ouro, vec3(0, 0, -5), 1.0);*/

    // 1. Carregue a textura
    auto textura_parede = make_shared<texture>("parede.ppm");

    // 2. Crie o material e atribua a textura
    auto material_parede = make_shared<material>(color(0, 0, 0), color(1, 1, 1), color(0, 0, 0), color(0, 0, 0), color(0, 0, 0), 10.0, 1.0);
    material_parede->tex = textura_parede;

    // 3. Defina os pontos
    point3 v0(0.0, 1.5, -2.0);
    point3 v1(-1.0, 0.0, -2.0);
    point3 v2(1.0, 0.0, -2.0);

    // 4. Adicione ao mundo usando o novo construtor com UVs, TRIANGULO COM TEXTURA
    world.add(make_shared<triangle>(
        v0, v2, v1,
        uv_coord{0.5, 1.0}, uv_coord{0.0, 0.0}, uv_coord{1.0, 0.0},
        material_parede)); // triangulo com textura de parede

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