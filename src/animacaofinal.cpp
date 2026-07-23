#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "node.hpp"
#include "matrix4.hpp"
#include "keyframe.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

// gera animação de 3 bolas quicando e duas piramides com textura quadriculada no chao, luz fixa, camera dando volta completa

// ---------- Gera os keyframes do quique de UMA bola ----------
// x_offset: posição horizontal fixa da bola
// z_pos: profundidade fixa da bola
// fase_inicial: desloca o "relógio" da bola para ela não quicar em sincronia com as outras
std::vector<keyframe> gerar_keyframes_quique(double duracao, double h0, double g,
                                             double raio_bola, double x_offset, double z_pos,
                                             double fase_inicial)
{
    std::vector<keyframe> kfs;

    double v0 = std::sqrt(2 * g * h0);
    double meio_periodo = v0 / g;

    // Começa a contagem "de trás pra frente" pela fase, para que cada bola já
    // esteja em um ponto diferente do quique quando o vídeo começar em t=0.
    double t = -fase_inicial;
    bool no_chao = (std::fmod(fase_inicial, 2 * meio_periodo) < meio_periodo);

    kfs.push_back({t, point3(x_offset, (no_chao ? 0.0 : h0) + raio_bola, z_pos),
                   no_chao ? easing_type::EASE_OUT : easing_type::EASE_IN});

    while (t < duracao + meio_periodo)
    {
        t += meio_periodo;
        no_chao = !no_chao;
        double altura = no_chao ? 0.0 : h0;
        easing_type facilitacao = no_chao ? easing_type::EASE_IN : easing_type::EASE_OUT;
        kfs.push_back({t, point3(x_offset, altura + raio_bola, z_pos), facilitacao});
    }

    return kfs;
}

// ---------- Constrói a geometria de UMA pirâmide (base quadrada) ----------
// A pirâmide é construída JÁ DE CABEÇA PARA BAIXO: o ápice fica embaixo
// (local y = -altura/2) and a base quadrada fica em cima (local y = +altura/2).
// Assim, girar o node em torno do eixo Y (rotate_y) gira a pirâmide "de ponta-cabeça"
// em torno do seu próprio eixo vertical, sem precisar de nenhuma rotação extra em X/Z.
std::vector<shared_ptr<hittable>> construir_piramide_invertida(shared_ptr<material> mat, double base, double altura)
{
    std::vector<shared_ptr<hittable>> triangulos;

    double meia_base = base / 2.0;
    double meia_altura = altura / 2.0;

    // 4 cantos da base (em cima) and the ápice (embaixo, no centro)
    point3 apice(0, -meia_altura, 0);
    point3 c1(-meia_base, meia_altura, -meia_base); // fundo-esquerda
    point3 c2(meia_base, meia_altura, -meia_base);  // fundo-direita
    point3 c3(meia_base, meia_altura, meia_base);   // frente-direita
    point3 c4(-meia_base, meia_altura, meia_base);  // frente-esquerda

    // Base (2 triângulos, vista de cima)
    triangulos.push_back(make_shared<triangle>(c1, c3, c2, mat));
    triangulos.push_back(make_shared<triangle>(c1, c4, c3, mat));

    // 4 faces laterais, ligando cada aresta da base ao ápice
    triangulos.push_back(make_shared<triangle>(c1, c2, apice, mat));
    triangulos.push_back(make_shared<triangle>(c2, c3, apice, mat));
    triangulos.push_back(make_shared<triangle>(c3, c4, apice, mat));
    triangulos.push_back(make_shared<triangle>(c4, c1, apice, mat));

    return triangulos;
}

int main()
{

    // ---------- 1. PARÂMETROS DO VÍDEO ----------
    const int fps = 30;
    const double duracao_segundos = 5.0;
    const int total_frames = int(fps * duracao_segundos); // 240 frames
    const double raio_bola = 0.6;

    // ---------- 2. KEYFRAMES DE CADA BOLA (fora de sincronia) ----------
    // h0 (altura máxima) e fase_inicial diferentes -> cada bola quica no seu próprio ritmo
    auto kf_esquerda = gerar_keyframes_quique(duracao_segundos, /*h0=*/2.4, /*g=*/9.8, raio_bola, /*x=*/-3.0, /*z=*/-5.0, /*fase=*/0.0);
    auto kf_meio = gerar_keyframes_quique(duracao_segundos, /*h0=*/3.2, /*g=*/9.8, raio_bola, /*x=*/0.0, /*z=*/-5.0, /*fase=*/0.35);
    auto kf_direita = gerar_keyframes_quique(duracao_segundos, /*h0=*/2.0, /*g=*/9.8, raio_bola, /*x=*/3.0, /*z=*/-5.0, /*fase=*/0.7);

    // ---------- 3. KEYFRAMES DA LUZ (FIXA) ----------
    // Posição fixa da luz. Dois keyframes idênticos para não mover ao longo do tempo.
    point3 pos_luz_fixa(0.0, 10.0, 10.0); // No alto e à frente
    std::vector<keyframe> kf_luz = {
        {0.0, pos_luz_fixa, easing_type::EASE_IN_OUT},
        {duracao_segundos, pos_luz_fixa, easing_type::EASE_IN_OUT}};

    // ---------- 4. KEYFRAMES DA CÂMERA (Movimento em linha reta da esquerda para a direita) ----------
    std::vector<keyframe> kf_camera = {
        // Inicia bem à esquerda, mantendo uma boa distância frontal (z = 4.0)
        {0.0, point3(-12.0, 3.0, 4.0), easing_type::LINEAR},

        // Ponto intermediário (passando pela esquerda/centro dos objetos)
        {1.25, point3(-4.0, 3.0, 4.0), easing_type::LINEAR},

        // Ponto central (passando bem na frente do centro da cena)
        {2.5, point3(0.0, 3.0, 4.0), easing_type::LINEAR},

        // Ponto intermediário direito
        {3.75, point3(4.0, 3.0, 4.0), easing_type::LINEAR},

        // Finaliza bem à direita da cena
        {duracao_segundos, point3(12.0, 3.0, 4.0), easing_type::LINEAR}};

    // ---------- 5. MATERIAIS ----------
    // Chão: textura quadriculada (xadrez.ppm) e difusa branca
    auto textura_chao = make_shared<texture>("xadrez.ppm");
    auto material_chao = make_shared<material>(
        color(0.1, 0.1, 0.1), color(1.0, 1.0, 1.0), color(0.1, 0.1, 0.1),
        color(0, 0, 0), color(0, 0, 0), 10.0, 1.0);
    material_chao->tex = textura_chao;

    // Bola da esquerda: textura de parede
    auto textura_parede = make_shared<texture>("parede.ppm");
    auto material_parede = make_shared<material>(color(0, 0, 0), color(1, 1, 1), color(0, 0, 0), color(0, 0, 0), color(0, 0, 0), 10.0, 1.0);
    material_parede->tex = textura_parede;

    // Bola do meio: ouro (mantido)
    auto material_ouro = make_shared<material>(color(0.2, 0.1, 0.0), color(0.8, 0.6, 0.2), color(1.0, 0.9, 0.5), color(0.8, 0.8, 0.5), color(0, 0, 0), 256.0, 1.0);

    // Bola da direita: azul (água) (mantido)
    auto material_azul = make_shared<material>(
        color(0.0, 0.0, 0.0),
        color(0.0, 0.0, 0.0),
        color(0.5, 0.5, 0.5),
        color(0.3, 0.3, 0.3),
        color(0.4, 0.7, 0.9),
        64.0,
        1.33);

    // Pirâmides: vermelho (mantido)
    auto material_vermelho = make_shared<material>(
        color(0.0, 0.0, 0.0),
        color(0.0, 0.0, 0.0),
        color(0.5, 0.5, 0.5),
        color(0.2, 0.2, 0.2),
        color(0.95, 0.8, 0.8),
        128.0,
        1.5);

    // Geometria das pirâmides (construída UMA vez só; reaproveitada em todos os frames) (mantido)
    const double piramide_base = 1.4;
    const double piramide_altura = 2.2;
    auto triangulos_piramide = construir_piramide_invertida(material_vermelho, piramide_base, piramide_altura);

    // Posições das pirâmides: redistribuídas para enquadrar a cena em diferentes profundidades
    const double x_piramide_esquerda = -6.0;
    const double z_piramide_esquerda = -4.0;
    const double x_piramide_direita = 6.0;
    const double z_piramide_direita = -6.0;
    const double y_piramide = piramide_altura / 2.0; // ápice tocando o chão (y=0)

    // Velocidade de rotação das pirâmides (graus por segundo) -- moderada (mantido, mas ajustado para 8s)
    const double velocidade_rotacao = 60.0; // ~1 volta completa ao longo dos 8s

    // ---------- 6. CÂMERA (parâmetros de imagem fixos; ALTA QUALIDADE) ----------
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 640;
    cam.samples_per_pixel = 10; // Mais amostras por pixel para melhor qualidade

    camera_parameters cam_1;
    cam_1.fov = 60;

    // ---------- 7. LOOP DE RENDERIZAÇÃO ----------
    for (int frame = 0; frame < total_frames; frame++)
    {
        double t = frame / double(fps);

        // Posições interpoladas a partir dos keyframes
        point3 pos_esquerda = keyframe_interpolate(kf_esquerda, t);
        point3 pos_meio = keyframe_interpolate(kf_meio, t);
        point3 pos_direita = keyframe_interpolate(kf_direita, t);
        point3 pos_luz = keyframe_interpolate(kf_luz, t);
        point3 pos_camera = keyframe_interpolate(kf_camera, t); // camera orbits

        // --- Monta a cena deste frame com mais objetos em profundidades variadas ---
        hittable_list world;
        world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_chao)); // chão

        // 1. Bola da Esquerda (tamanho padrão, mais próxima)
        auto bola_esquerda = make_shared<node>();
        bola_esquerda->add(make_shared<sphere>(point3(0, 0, 0), raio_bola, material_parede));
        bola_esquerda->translate(vec3(pos_esquerda.x(), pos_esquerda.y(), pos_esquerda.z()));
        bola_esquerda->update_transforms(matrix4::identity());
        world.add(bola_esquerda);

        // 2. Bola do Meio (tamanho médio, mais ao fundo)
        auto bola_meio = make_shared<node>();
        bola_meio->add(make_shared<sphere>(point3(0, 0, 0), raio_bola, material_ouro));
        bola_meio->translate(vec3(pos_meio.x(), pos_meio.y(), pos_meio.z()));
        bola_meio->update_transforms(matrix4::identity());
        world.add(bola_meio);

        // 3. Bola da Direita (tamanho padrão)
        auto bola_direita = make_shared<node>();
        bola_direita->add(make_shared<sphere>(point3(0, 0, 0), raio_bola, material_azul));
        bola_direita->translate(vec3(pos_direita.x(), pos_direita.y(), pos_direita.z()));
        bola_direita->update_transforms(matrix4::identity());
        world.add(bola_direita);

        // --- NOVOS OBJETOS ADICIONAIS ---

        // 4. Bola Gigante ao Fundo (uma bola bem grande e estática lá atrás)
        auto bola_gigante_fundo = make_shared<node>();
        bola_gigante_fundo->add(make_shared<sphere>(point3(0, 0, 0), 2.5, material_parede)); // Raio 2.5 (bem grande)
        bola_gigante_fundo->translate(vec3(-2.0, 2.5, -12.0));                               // Bem no fundo e à esquerda
        bola_gigante_fundo->update_transforms(matrix4::identity());
        world.add(bola_gigante_fundo);

        // 5. Bola Média Estática Adicional (no lado direito, profundidade intermediária)
        auto bola_extra_direita = make_shared<node>();
        bola_extra_direita->add(make_shared<sphere>(point3(0, 0, 0), 1.0, material_ouro)); // Raio maior (1.0)
        bola_extra_direita->translate(vec3(6.5, 1.0, -6.0));
        bola_extra_direita->update_transforms(matrix4::identity());
        world.add(bola_extra_direita);

        // 6. Bola Pequena Flutuando/Mais Próxima (para dar contraste de escala)
        auto bola_pequena_frente = make_shared<node>();
        bola_pequena_frente->add(make_shared<sphere>(point3(0, 0, 0), 0.4, material_azul)); // Bola menor
        bola_pequena_frente->translate(vec3(-5.0, 0.4, -2.5));                              // Mais próxima da câmera
        bola_pequena_frente->update_transforms(matrix4::identity());
        world.add(bola_pequena_frente);

        // --- Pirâmides invertidas originais, com posições desalinhadas ---
        double angulo_piramide = t * velocidade_rotacao;

        auto piramide_esquerda = make_shared<node>();
        for (auto &tri : triangulos_piramide)
        piramide_esquerda->add(tri);
        piramide_esquerda->rotate_y(angulo_piramide);
        piramide_esquerda->translate(vec3(x_piramide_esquerda, y_piramide, z_piramide_esquerda));
        piramide_esquerda->update_transforms(matrix4::identity());
        world.add(piramide_esquerda);

        auto piramide_direita = make_shared<node>();
        for (auto &tri : triangulos_piramide)
        piramide_direita->add(tri);
        piramide_direita->rotate_y(angulo_piramide);
        piramide_direita->translate(vec3(x_piramide_direita, y_piramide, z_piramide_direita));
        piramide_direita->update_transforms(matrix4::identity());
        world.add(piramide_direita);

        // --- Atualiza luz e câmera deste frame ---
        cam.posicao_luz = pos_luz; // posição fixa
        cam_1.eye_e = pos_camera;  // camera mechendo

        // --- Nome do arquivo ---
        std::ostringstream nome;
        nome << "frames_final/frame_" << std::setw(4) << std::setfill('0') << frame << ".ppm";

        std::clog << "Frame " << frame << "/" << total_frames
                  << "  luz.x=" << pos_luz.x() << "  cam.pos=" << pos_camera.x() << "," << pos_camera.y() << "," << pos_camera.z() << "\n";

        cam.render(world, cam_1, nome.str());
    }

    std::cout << "Todos os " << total_frames << " frames foram gerados em frames_final/\n";
    return 0;
}