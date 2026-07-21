#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.hpp"
#include <fstream>
#include <vector>
#include <string>

inline double clamp_val(double val, double min, double max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

class texture
{
    int width, height;
    std::vector<unsigned char> data;

public:
    texture(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        std::string header;
        int max_val;
        file >> header >> width >> height >> max_val;
        file.get(); // Consome o \n

        data.resize(width * height * 3);
        file.read(reinterpret_cast<char *>(data.data()), data.size());
    }

    color value(double u, double v) const
    {
        // Clamp para garantir que está entre 0 e 1
        u = clamp_val(u, 0.0, 1.0);
        v = clamp_val(v, 0.0, 1.0);

        int i = static_cast<int>(u * width);
        int j = static_cast<int>((1.0 - v) * height); // Inverte Y para PPM

        if (i >= width)
            i = width - 1;
        if (j >= height)
            j = height - 1;

        auto pixel = &data[3 * (j * width + i)];

        return color(pixel[0] / 255.0, pixel[1] / 255.0, pixel[2] / 255.0);
    }
};

#endif