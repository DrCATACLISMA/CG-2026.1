# Ray Tracer em C++ com Sistema de Animação e Grafo de Cena

Este repositório contém um **Ray Tracer completo desenvolvido do zero em C++**, implementando algoritmos clássicos de Computação Gráfica, renderização recursiva, texturização, transformações hierárquicas, suporte a malhas `.obj` e animação baseada em keyframes.

O projeto simula o comportamento físico da luz lançando raios da câmera para a cena, calculando interseções geométricas, iluminação local, sombras, reflexões e refrações.

---

## 🚀 Sobre o Projeto

O motor de ray tracing implementa as seguintes técnicas fundamentais:
- **Ray Tracing Recursivo (Estilo Whitted):** Suporte completo a reflexões (espelhos) e refrações (vidro/água) via Lei de Snell.
- **Iluminação de Phong:** Componentes ambiente, difusa e especular calculadas para múltiplas fontes de luz.
- **Shadow Rays:** Detecção precisa de sombras duras através de raios de sombra lançados em direção à luz.
- **Mapeamento de Texturas:** Suporte a imagens em formato PPM aplicadas via coordenadas UV.
- **Leitor de Malhas OBJ (`mesh_loader.hpp`):** Parser simplificado para importação e renderização de arquivos de malha poligonal externos em formato `.obj`.
- **Grafo de Cena Hierárquico:** Sistema de nós com matrizes de transformação 4x4 (translação, escala e rotação) para organizar objetos de forma aninhada.
- **Sistema de Animação por Keyframes:** Curvas de animação com diferentes tipos de *easing* (Linear, Ease-In, Ease-Out, Ease-In-Out).

---

## 📂 Estrutura do Repositório

O projeto é modularizado em arquivos de cabeçalho (`.hpp`) e fontes (`.cpp`) que se dividem em:
- **Matemática e Geometria:** `vector.hpp`, `ray.hpp`, `matrix4.hpp`, `interval.hpp`
- **Objetos e Interseção:** `hittable.hpp`, `hittable_list.hpp`, `sphere.hpp`, `triangle.hpp`, `node.hpp`
- **Materiais e Texturas:** `material.hpp`, `texture.hpp`
- **Importação de Malhas:** `mesh_loader.hpp`
- **Animação:** `keyframe.hpp`
- **Renderização e Câmera:** `camera.hpp`
- **Arquivo Principal Atual da Animação Final:** `animacaofinal.cpp`

---

## 🎥 Vídeos de Demonstração e Versões Anteriores

No repositório, você encontrará demonstrações em vídeo das animações geradas pelo motor, divididas entre a versão atual e as versões anteriores que serviram de base:

- **Animação Final (`animacaofinal.cpp`):** Demonstração atual e completa combinando múltiplas bolas quicando fora de sincronia, pirâmides invertidas girando, textura quadriculada no chão (`xadrez.ppm`), iluminação fixa e câmera em movimento orbital de 360 graus ao redor da cena.
- **Outras Animações (Versões Anteriores):**
  - Gerada a partir de `main_projeto.cpp`: Versão inicial focada na estruturação da cena estática, hierarquia de nós e testes de materiais variados.
  - Gerada a partir de `tres_bolas.cpp`: Versão intermediária com três bolas quicando em ritmos diferentes, pirâmides e câmera com aproximação frontal.
  - Gerada a partir de `bouncing_ball_keyframe.cpp`: Versão de testes focada no comportamento da animação por keyframes e curvas de *easing* aplicadas a uma única bola.

---

## 🛠️ Como Compilar e Executar (`animacaofinal.cpp`)

Para compilar e gerar a animação final descrita no arquivo `animacaofinal.cpp`, siga o fluxo recomendado abaixo utilizando o terminal:

### 1. Compilação do Código C++
Utilize o compilador `g++` com otimizações de desempenho ativadas (`-O2` ou `-O3`) e suporte ao padrão C++17:

```bash
g++ -O2 animacaofinal.cpp -o animacaofinal
```

### 2. Criação do Diretório de Quadros e Execução
Crie a pasta de destino para armazenar os arquivos `.ppm` renderizados quadro a quadro e execute o programa:

```bash
mkdir -p frames_final ; ./animacaofinal
```
*(Nota: Certifique-se de que as texturas necessárias, como `xadrez.ppm` e `parede.ppm`, estejam presentes no mesmo diretório do executável).*

### 3. Geração do Vídeo Final com FFmpeg
Após a renderização de todos os quadros, utilize a ferramenta **FFmpeg** para unir as imagens em um arquivo de vídeo fluido a 30 FPS:

```bash
ffmpeg -framerate 30 -i frames_final/frame_%04d.ppm -c:v libx264 -pix_fmt yuv420p saida.mp4
```

---

## 📚 Referência Acadêmica e Inspiração
O projeto tem como base conceitual os fundamentos apresentados na literatura clássica de computação gráfica, com destaque para a série de livros *Ray Tracing in One Weekend* adaptada para um motor gráfico próprio com suporte a animação, malhas e grafos de cena.
