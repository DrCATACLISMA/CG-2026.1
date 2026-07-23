#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.hpp"
#include "material.hpp"
#include <fstream>
#include <vector>

struct light {
    point3 position;
    color intensity;
};

struct camera_parameters
{
    double fov;
    point3 eye_e{0, 0, 0}; // valor padrão
    double ny;
    double nx = -1;
    // vec3 u;
    // look_at
};

class camera
{
public:
    // proporção da imagem padrao
    double aspect_ratio = 1.0; 
    int image_width = 100;

    /*
        valores padrões
        double aspect_ratio = 16.0/9.0;
        int image_width = 400;
    */

    // Número de amostras por pixel (quanto maior, mais suave, mas mais lento).
    // Agora é membro público para poder ser ajustado de fora (ex: menor durante testes de animação).
    int samples_per_pixel = 200; // aumento do samples per pixel

    // Posição da luz principal da cena. Público para poder ser animada frame a frame
    // (ex: mover a luz da direita para a esquerda ao longo do vídeo).
    point3 posicao_luz{5, 5, 0};

    void render(const hittable &world, camera_parameters camp, const std::string &filename = "imagem_CG_FINAL.ppm")
    {

        initialize(camp);
        // criação de objeto da imagem (nome agora é parametrizável, útil para gerar frame_0000.ppm, frame_0001.ppm...)
        std::ofstream imagem{filename};

        imagem << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";

        int max_depth = 5; 
        
        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                color pixel_color(0, 0, 0);

                // Substitua o loop s por este:
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    // Calcula o deslocamento aleatório diretamente na posição do pixel
                    auto pixel_sample = pixel00_loc + ((i + random_double()) * pixel_delta_u) + ((j + random_double()) * pixel_delta_v);

                    auto ray_direction = pixel_sample - camera_center;
                    ray r{camera_center, ray_direction};
                    pixel_color += ray_color(r, world, max_depth);
                }

                //Grava a média das cores acumuladas
                write_color(imagem, pixel_color / samples_per_pixel);
            }
        }

        std::clog << "\rFeito.                 \n";
    }

private:
    int image_height;
    point3 camera_center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;

    void initialize(camera_parameters camp)
    {
        // calculo do tamanho da imagem, deve ser pelo menos 1
        image_height = int(image_width / aspect_ratio); // calculo do ny usando o aspect e o nx
        image_height = (image_height < 1) ? 1 : image_height;

        camera_center = camp.eye_e;

        // fov
        auto focal_length = 1.0;

        // Converter o FOV (em graus) para radianos 
        auto theta = degrees_to_radians(camp.fov); 
        auto h = std::tan(theta / 2.0);

        auto viewport_height = 2.0 * h * focal_length;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // -------------------------------------------------
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // calcula o tamanho do pixel no espaço 3D
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // viewport_v é o up

        // calcula a posição do canto superior esquerdo da viewport
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        // Calcula o centro do pixel (0,0)
        // viewport_upper_left é o canto do pixel, não o centro.
        // lançar o raio pelo centro do pixel
    }

    // renderizando um gradiente 
    /* FUNÇÃO ANTIGA
        color ray_color(const ray &r, const hittable &world) const
        {
            hit_record rec;
            if (world.hit(r, interval(0, infinity), rec))
            {
                return 0.5 * (rec.normal + color(1, 1, 1));
            }

            // unit_vector -> função para a transformação de vetor para o unitario
            // r.direction -> getter com retorno da direção do raio
            vec3 unit_direction = unit_vector(r.direction());

            // getter do componente y do vetor
            auto a = 0.5 * (unit_direction.y() + 1.0);

            // retorna uma cor
            return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        }
    */

    color ray_color(const ray &r, const hittable &world, int depth) const
    {
        // Se excedeu o limite de rebatimentos, a luz do reflexo não contribui mais
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;
        
        // IMPORTANTE: Alterado de 0 para 0.001 para evitar o "Shadow Acne"
        if (world.hit(r, interval(0.001, infinity), rec))
        {
            
            // --- 1. DEFINIÇÕES DA CENA E MATERIAL ---
            // Luz principal: posição vem do membro posicao_luz (configurável/animável de fora)
            light luz_principal = { posicao_luz, color(1.0, 1.0, 1.0) };
            std::vector<light> luzes = { luz_principal };
            
            color luz_ambiente_global(1.0, 1.0, 1.0);

            // Propriedades do Material (Aplicado a todas as esferas por enquanto)
            color Ka = rec.mat->Ka;  // Constante Ambiente
                                     // Se o material tem textura, substitui a constante Kd
            color Kd = rec.mat->Kd;  // Constante Difusa 

            if (rec.mat->tex)
            {
                Kd = rec.mat->tex->value(rec.u, rec.v);
            } 

            color Ks = rec.mat->Ks;  // Constante Especular (Brilho branco)
            double shininess = rec.mat->shininess; // Concentração do brilho (n)
            color Kr = rec.mat->Kr; // Reflexão
            color emissao(0, 0, 0);  // I_E

            // --- 2. CÁLCULO BASE ---
            // I = I_E + K_A * I_A
            color cor_final = emissao + (Ka * luz_ambiente_global);

            // Vetores necessários
            vec3 V = unit_vector(camera_center - rec.p); // Vetor Câmera
            vec3 N = unit_vector(rec.normal);            // Vetor Normal

            // --- 3. SOMATÓRIO DAS LUZES ---
            for (const auto& luz : luzes) {
                vec3 direcao_luz = luz.position - rec.p;
                double distancia_luz = direcao_luz.length(); 
                vec3 L = unit_vector(direcao_luz); // Vetor Luz

                // A. RAIO DE SOMBRA (Shadow Ray - S_L)
                // Lança um raio do ponto de interseção até a luz.
                // O intervalo max é 'distancia_luz' para não detectar objetos que estejam atrás da luz.
                ray shadow_ray(rec.p, L);
                hit_record shadow_rec;
                double S_L = 1.0; 
                
                if (world.hit(shadow_ray, interval(0.001, distancia_luz), shadow_rec)) {
                    S_L = 0.0; // Objeto bloqueou a luz, está na sombra
                }

                // B. CÁLCULO DE ILUMINAÇÃO DIRETA
                if (S_L > 0.0) {
                    // Termo Difuso: K_D * (N • L)
                    double N_dot_L = std::fmax(0.0, dot(N, L));
                    color difuso = Kd * N_dot_L;

                    // Termo Especular: K_S * (V • R)^n
                    color especular(0, 0, 0);
                    if (N_dot_L > 0.0) { // O brilho só existe se a face está virada para a luz
                        // Cálculo do vetor R de reflexão: 2 * (N • L) * N - L
                        vec3 R = unit_vector((2.0 * N_dot_L * N) - L);
                        double V_dot_R = std::fmax(0.0, dot(V, R));
                        double brilho = std::pow(V_dot_R, shininess);
                        especular = Ks * brilho;
                    }

                    // Soma do resultado da luz atual: (Difuso + Especular) * S_L * I_L
                    cor_final += (difuso + especular) * luz.intensity * S_L;
                }
            }

            if (rec.mat->Kt.x() > 0 || rec.mat->Kt.y() > 0 || rec.mat->Kt.z() > 0)
            {
                vec3 incident = unit_vector(r.direction());
                vec3 normal = rec.normal;
                double ior = rec.mat->ior;
                double eta; // Razão dos índices de refração

                // Verifica se estamos entrando ou saindo do objeto
                if (dot(incident, normal) < 0)
                {
                    eta = 1.0 / ior; // Entrando
                }
                else
                {
                    eta = ior / 1.0;  // Saindo
                    normal = -normal; // Inverte a normal
                }

                double cos_i = -dot(normal, incident);
                double sin2_t = eta * eta * (1.0 - cos_i * cos_i);

                // Reflexão Interna Total (TIR)
                if (sin2_t > 1.0)
                {
                    // Se ocorrer reflexão total, trata apenas como reflexão
                    vec3 reflected_dir = incident - (2.0 * dot(incident, rec.normal) * rec.normal);
                    ray reflected_ray(rec.p, reflected_dir);
                    cor_final += rec.mat->Kr * ray_color(reflected_ray, world, depth - 1);
                }
                else
                {
                    // Refração real (Lei de Snell)
                    double cos_t = std::sqrt(1.0 - sin2_t);
                    vec3 refracted_dir = eta * incident + (eta * cos_i - cos_t) * normal;
                    ray refracted_ray(rec.p, refracted_dir);

                    // Chamada recursiva para refração
                    cor_final += rec.mat->Kt * ray_color(refracted_ray, world, depth - 1);
                }
            }

            // --- NOVO: RECURSÃO (REFLEXÃO) ---
            // Verifica se o material tem alguma capacidade de reflexão
            if (Kr.x() > 0 || Kr.y() > 0 || Kr.z() > 0)
            {
                // Direção do raio incidente (o raio que bateu no objeto)
                vec3 incident = unit_vector(r.direction());

                // Fórmula da reflexão: R = I - 2(I • N)N
                vec3 reflected_dir = incident - (2.0 * dot(incident, N) * N);

                // Cria o novo raio a partir do ponto de colisão
                ray reflected_ray(rec.p, reflected_dir);

                // Chamada recursiva: soma a cor refletida multiplicada pela taxa de reflexão do material
                cor_final += Kr * ray_color(reflected_ray, world, depth - 1);
            }

            // --- 4. PREVENÇÃO DE ESTOURO DE COR ---
            // Garante que o valor RGB nunca passe de 1.0 (branco puro)
            cor_final.e[0] = std::fmin(1.0, cor_final.x());
            cor_final.e[1] = std::fmin(1.0, cor_final.y());
            cor_final.e[2] = std::fmin(1.0, cor_final.z());

            return cor_final;
        }

        // Fundo da imagem (Céu gradiente)
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

};

#endif