#include "color.hpp"
#include "vector.hpp"
#include "ray.hpp"

#include <iostream>

// renderizando um gradiente
color ray_color(const ray& r) {

    // unit_vector -> função para a transformação de vetor para o unitario
    // r.direction -> getter com retorno da direção do raio
    vec3 unit_direction = unit_vector(r.direction());

    // getter do componente y do vetor
    auto a = 0.5*(unit_direction.y() + 1.0);

    // retorna uma cor
    // ===== PERGUNTAR PARA O RENER
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {

    // proporção da imagem
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // calculo do tamanho da imagem, deve ser pelo menos 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    double focal_length = 1.0;
    // altura viewport
    double viewport_height = 2.0;
    // largura viewport
    double viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0, 0, 0);

    // largura e altura da tela, percorre pixel por pixel, orientação da imagem
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // calcula o tamanho do pixel no espaço 3D
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // calcula a posição do canto superior esquerdo da viewport
    auto viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length) 
                             - viewport_u/2 - viewport_v/2;
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

    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    // Calcula o centro do pixel (0,0)
    // viewport_upper_left é o canto do pixel, não o centro.
    // lançar o raio pelo centro do pixel

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) { // perguntar para o Rener

            // pixel00_loc é a posição do primeiro pixel, pixel_delta_u e pixel_delta_v espaço entre os pixels
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r{camera_center, ray_direction}; // construtor

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";
}