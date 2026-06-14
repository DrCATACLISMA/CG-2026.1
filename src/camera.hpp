#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.hpp"
#include <fstream>
#include <vector>

// fov, nx, ny, look_at, up

/*
    Terefas importantes:
        Construir e enviar raios para o mundo.
        Utilizar os resultados desses raios para construir a imagem renderizada
*/

/*
    fov:
        double focal_length = 1.0;
    olho e:
        point3 camera_center = point3(0, 0, 0);
    ny:
        double viewport_height = 2.0;
    nx:
        double viewport_width;
    up:
        vec3 viewport_v;
    look_at:

*/

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
    /*
        representa a proporção ideal, PODE NÃO SER A PROPORÇÃO REAL
        entre image_width e image_height

        `y` image_heighté arredondado para baixo para o inteiro mais próximo, o que pode aumentar a proporção.
        não permitimos image_heightque `y` seja menor que um, o que também pode alterar a proporção real.
        
    */
    int image_width = 100;

    /*
        valores padrões
        double aspect_ratio = 16.0/9.0;
        int image_width = 400;
    */

    void render(const hittable &world, camera_parameters camp)
    {

        initialize(camp);
        // criação de objeto da imagem
        std::ofstream imagem{"imagem_CG.ppm"};

        imagem << "P3\n"
               << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                // pixel00_loc é a posição do primeiro pixel, pixel_delta_u e pixel_delta_v espaço entre os pixels
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - camera_center;
                ray r{camera_center, ray_direction}; // construtor

                color pixel_color = ray_color(r, world);
                write_color(imagem, pixel_color);
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

        // olho e
        // ANTIGO: camera_center = point3(0, 0, 0);

        camera_center = camp.eye_e;

        // fov
        auto focal_length = 1.0;
        // MUDADO PARA O ANTIGO !!! <-----
        // auto focal_length = camp.fov;
        // altura viewport
        // ANTIGO: auto viewport_height = 2.0; // ny

        // Converter o FOV (em graus) para radianos <---- SUJEITO A DELEÇÃO!!!
        auto theta = degrees_to_radians(camp.fov); 
        auto h = std::tan(theta / 2.0);

        auto viewport_height = 2.0 * h * focal_length;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        /*
                    if (camp.nx == -1)
                        viewport_width = viewport_height * (double(image_width) / image_height); // nx
                    else
                        viewport_width = camp.nx;
        */
        // auto viewport_width = viewport_height * (double(image_width) / image_height); // nx

        // largura e altura da tela, percorre pixel por pixel, orientação da imagem
        /*
            VETORES:
            Vu: viewport_u
            Vv: viewport_v

            Vu: esquerda para direita
            Vv: superior para inferior
        */

        // -------------------------------------------------
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // calcula o tamanho do pixel no espaço 3D
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // viewport_v é o up

        // calcula a posição do canto superior esquerdo da viewport
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        // calcula o centro do pixel (0,0)
        /*
            1. COMEÇA NO CENTRO da câmera, camera_center (0,0,0)
            2. ANDA PRA FRENTE (eixo Z negativo), - vec3(0, 0, focal_length)
            3. VAI METADE PARA ESQUERDA,
                - viewport_u/2
                    Porque viewport_u vai pra direita
                    então o negativo leva para a esquerda
            4. VAI METADE PRA CIMA
                - viewport_v/2
                    Lembra: viewport_v aponta para baixo
                    então o negativo sobe
            5. RESULTADO:
                VOCÊ CHEGA NO CANTO SUPERIOR ESQUERDO DA TELA
        */

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

    color ray_color(const ray &r, const hittable &world) const
    {
        hit_record rec;
        
        // IMPORTANTE: Alterado de 0 para 0.001 para evitar o "Shadow Acne"
        if (world.hit(r, interval(0.001, infinity), rec))
        {
            // --- 1. DEFINIÇÕES DA CENA E MATERIAL ---
            // Luz principal posicionada no alto e à direita
            light luz_principal = { point3(5, 5, 0), color(1.0, 1.0, 1.0) };
            std::vector<light> luzes = { luz_principal };
            
            color luz_ambiente_global(1.0, 1.0, 1.0);

            // Propriedades do Material (Aplicado a todas as esferas por enquanto)
            color Ka(0.1, 0.1, 0.1); // Constante Ambiente
            color Kd(0.2, 0.5, 0.8); // Constante Difusa (Um tom de azul agradável)
            color Ks(0.8, 0.8, 0.8); // Constante Especular (Brilho branco)
            double shininess = 64.0; // Concentração do brilho (n)
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