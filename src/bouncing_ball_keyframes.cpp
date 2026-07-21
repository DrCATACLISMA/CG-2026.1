#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include "node.hpp"
#include "matrix4.hpp"
#include "keyframe.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

// ---------- Gera os keyframes da bola quicando ----------
// Em vez de calcular a posição da bola frame a frame com uma fórmula fechada,
// definimos só as poses-chave: "tocando o chão" e "no topo do quique",
// alternadamente, cobrindo toda a duração do vídeo.
std::vector<keyframe> gerar_keyframes_quique(double duracao, double h0, double g, double raio_bola)
{
    std::vector<keyframe> kfs;

    double v0 = std::sqrt(2 * g * h0);   // velocidade inicial necessária para atingir h0
    double meio_periodo = v0 / g;        // tempo do chão até o topo (metade do período)

    double t = 0.0;
    bool no_chao = true;

    // Keyframe inicial: bola no chão em t=0, subindo (EASE_OUT: rápida no início, desacelera no topo)
    kfs.push_back({t, point3(0, raio_bola, -3), easing_type::EASE_OUT});

    // Gera um pouco além da duração (+ meio_periodo) para garantir interpolação
    // válida até o último frame do vídeo.
    while (t < duracao + meio_periodo)
    {
        t += meio_periodo;
        no_chao = !no_chao;

        double altura = no_chao ? 0.0 : h0;

        // Se a bola está indo em direção ao chão (vai ficar "no_chao"), a queda acelera -> EASE_IN
        // Se está indo em direção ao topo, ela desacelera -> EASE_OUT
        easing_type facilitacao = no_chao ? easing_type::EASE_IN : easing_type::EASE_OUT;

        kfs.push_back({t, point3(0, altura + raio_bola, -3), facilitacao});
    }

    return kfs;
}

int main()
{
    // ---------- 1. PARÂMETROS DO VÍDEO ----------
    const int fps = 30;
    const double duracao_segundos = 5.0;
    const int total_frames = int(fps * duracao_segundos); // 150 frames

    // ---------- 2. PARÂMETROS DO QUIQUE (usados só para GERAR os keyframes) ----------
    const double g = 9.8;
    const double h0 = 3.0;
    const double raio_bola = 0.5;

    std::vector<keyframe> keyframes_bola = gerar_keyframes_quique(duracao_segundos, h0, g, raio_bola);

    std::cout << "Gerados " << keyframes_bola.size() << " keyframes:\n";
    for (const auto &k : keyframes_bola)
        std::cout << "  t=" << k.time << "s  y=" << k.position.y() << "\n";

    // ---------- 3. MATERIAIS ----------
    auto material_chao = make_shared<material>(
        color(0.1, 0.1, 0.1), color(0.2, 0.8, 0.2), color(0.1, 0.1, 0.1),
        color(0, 0, 0), color(0, 0, 0), 10.0, 1.0);

    auto material_bola = make_shared<material>(
        color(0.2, 0.0, 0.0), color(0.8, 0.1, 0.1), color(1.0, 1.0, 1.0),
        color(0, 0, 0), color(0, 0, 0), 64.0, 1.0);

    // ---------- 4. CÂMERA (fixa durante todo o vídeo) ----------
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 120;      // baixe para testar rápido; aumente (ex: 854) na versão final
    cam.samples_per_pixel = 4;  // idem

    camera_parameters cam_1;
    cam_1.fov = 60;
    cam_1.eye_e = point3(0, 3, 10);

    // ---------- 5. LOOP DE RENDERIZAÇÃO ----------
    for (int frame = 0; frame < total_frames; frame++)
    {
        double t = frame / double(fps); // instante de tempo (s) deste frame

        // A posição da bola agora vem da INTERPOLAÇÃO entre os keyframes, não de uma fórmula direta
        point3 pos_bola = keyframe_interpolate(keyframes_bola, t);

        // --- Monta a cena deste frame do zero ---
        hittable_list world;
        world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_chao)); // chão

        auto bola = make_shared<node>();
        bola->add(make_shared<sphere>(point3(0, 0, 0), raio_bola, material_bola));
        bola->translate(vec3(pos_bola.x(), pos_bola.y(), pos_bola.z()));
        bola->update_transforms(matrix4::identity());
        world.add(bola);

        // --- Nome do arquivo: frame_0000.ppm, frame_0001.ppm, ... ---
        std::ostringstream nome;
        nome << "frames_kf/frame_" << std::setw(4) << std::setfill('0') << frame << ".ppm";

        std::clog << "Renderizando frame " << frame << "/" << total_frames
                   << " (y=" << pos_bola.y() << ")\n";

        cam.render(world, cam_1, nome.str());
    }

    std::cout << "Todos os " << total_frames << " frames foram gerados em frames_kf/\n";
    return 0;
}
