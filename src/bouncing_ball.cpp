#include "rtweekend.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include "node.hpp"
#include "matrix4.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

int main()
{
    // ---------- 1. PARÂMETROS DO VÍDEO ----------
    const int fps = 30;
    const double duracao_segundos = 5.0;
    const int total_frames = int(fps * duracao_segundos); // 150 frames

    // ---------- 2. FÍSICA SIMPLIFICADA DO QUIQUE ----------
    // Sem elasticidade / sem perda de energia: a bola sempre volta à mesma
    // altura máxima h0. É só um movimento parabólico (gravidade) que se repete.
    const double g = 9.8;                        // "gravidade" na escala da cena
    const double h0 = 3.0;                        // altura máxima do quique
    const double v0 = std::sqrt(2 * g * h0);       // velocidade inicial para atingir h0
    const double periodo = 2 * v0 / g;             // tempo entre dois toques no chão
    const double raio_bola = 0.5;

    std::cout << "Periodo de um quique: " << periodo << "s -> ~"
               << (duracao_segundos / periodo) << " quiques em " << duracao_segundos << "s\n";

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
    cam.image_width = 1280;       // baixe para testar rápido; aumente (ex: 854) na versão final
    cam.samples_per_pixel = 32;  // idem: menos amostras = mais rápido, porém mais "ruído"

    camera_parameters cam_1;
    cam_1.fov = 60;
    cam_1.eye_e = point3(0, 3, 10);

    // ---------- 5. LOOP DE RENDERIZAÇÃO: 1 chamada de render() por frame ----------
    for (int frame = 0; frame < total_frames; frame++)
    {
        double t = frame / double(fps); // instante de tempo (s) deste frame
        double tau = std::fmod(t, periodo); // tempo desde o último toque no chão

        // y(tau) = v0*tau - 1/2*g*tau^2  -> movimento parabólico do quique
        double y = v0 * tau - 0.5 * g * tau * tau;
        if (y < 0) y = 0; // segurança numérica

        // --- Monta a cena deste frame do zero ---
        hittable_list world;
        world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_chao)); // chão

        auto bola = make_shared<node>();
        bola->add(make_shared<sphere>(point3(0, 0, 0), raio_bola, material_bola));
        bola->translate(vec3(0, y + raio_bola, -3)); // +raio para a bola não afundar no chão
        bola->update_transforms(matrix4::identity());
        world.add(bola);

        // --- Nome do arquivo: frame_0000.ppm, frame_0001.ppm, ... ---
        std::ostringstream nome;
        nome << "frames/frame_" << std::setw(4) << std::setfill('0') << frame << ".ppm";

        std::clog << "Renderizando frame " << frame << "/" << total_frames
                   << " (y=" << y << ")\n";

        cam.render(world, cam_1, nome.str());
    }

    std::cout << "Todos os " << total_frames << " frames foram gerados em frames/\n";
    return 0;
}
