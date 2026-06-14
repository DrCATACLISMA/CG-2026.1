#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "rtweekend.hpp"
#include "hittable_list.hpp"
#include "triangle.hpp"
#include "material.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Esta função lê um arquivo OBJ e adiciona todos os triângulos à lista de objetos da cena
inline bool load_obj(const std::string &filename, hittable_list &world,
                     shared_ptr<material> mat, // <--- ADICIONADO
                     vec3 offset = vec3(0, 0, 0), double scale = 1.0)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo " << filename << "\n";
        return false;
    }

    std::vector<point3> vertices;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Lendo vértices
            double x, y, z;
            iss >> x >> y >> z;
            // Aplica escala e posição e salva o vértice
            vertices.push_back(point3(x * scale, y * scale, z * scale) + offset);
        } 
        else if (prefix == "f") {
            // Lendo as faces (triângulos)
            int v_idx[3];
            std::string vertex_data;
            
            for (int i = 0; i < 3; i++) {
                iss >> vertex_data;
                // Extrai apenas o primeiro número antes de uma possível '/'
                // (Alguns OBJs têm f 1/1/1, nós queremos só o primeiro 1)
                std::size_t pos = vertex_data.find('/');
                if (pos != std::string::npos) {
                    vertex_data = vertex_data.substr(0, pos);
                }
                v_idx[i] = std::stoi(vertex_data) - 1; // Subtrai 1 porque OBJ começa do índice 1
            }

            // Adiciona o triângulo à cena
            world.add(make_shared<triangle>(
                vertices[v_idx[0]],
                vertices[v_idx[1]],
                vertices[v_idx[2]],
                mat));
        }
    }

    std::cout << "Mesh carregada com sucesso: " << filename << "\n";
    return true;
}

#endif