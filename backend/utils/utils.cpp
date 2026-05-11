#include "utils.hpp"
#include <cmath>
#include "../core/node/node.hpp"
#include "../core/definitions.hpp"

std::vector<SDL_Vertex> circleFan(Vector2 position, float radius, Color color, int numSides)
{
    int numVertices = numSides + 2;
    float pi2 = 2.0f * 3.141592653f;
    SDL_FColor col = static_cast<SDL_FColor>(color);

    std::vector<SDL_Vertex> vertices(numVertices);
    vertices[0].position.x = position.x;
    vertices[0].position.y = position.y;
    vertices[0].color = col;

    for (int i = 1; i < numVertices; i++)
    {
        vertices[i].position.x = position.x + (radius * cos(i * pi2 / numSides));
        vertices[i].position.y = position.y + (radius * sin(i * pi2 / numSides));
        vertices[i].color = col;
    }

    return vertices;
}


void drawCircle(Vector2 position, float radius, Color modulate, Window *window, int numSides) {
    std::vector<SDL_Vertex> vrt = circleFan(position, radius, modulate, numSides);

    // fan triangulation from vertex 0
    std::vector<int> indices;
    for (int i = 1; i < numSides + 2 - 1; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(window->renderer, nullptr, vrt.data(), numSides + 2, indices.data(), static_cast<int>(indices.size()));
}