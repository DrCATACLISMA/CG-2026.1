#include "color.hpp"
#include "vector.hpp"

#include <iostream>

int main() {

    // proporção da imagem
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // calculo do tamanho da imagem, deve ser pelo menos 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width)/image_height);

    int image_width = 256;
    int image_height = 256;

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_color = color(double(i)/(image_width-1), double(j)/(image_height-1), 0); // contrutor do vect
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";
}