#include "utils.hpp"
#include <cmath>
#include "logger.hpp"
#include "../core/node/node.hpp"
#include "../core/definitions.hpp"
#include "../services/physics/definitions.hpp"

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

std::vector<SDL_Vertex> semiCircleFan(Vector2 position, float radius, float direction, Color color, int numSides)
{
    int numVertices = numSides + 2;
    float pi = 3.141592653f;
    float startAngle = direction - 90.0f;
    float endAngle = direction + 90.0f;

    float startRad = startAngle * pi / 180.0f;
    float endRad = endAngle * pi / 180.0f;
    float angleStep = (endRad - startRad) / numSides;
    SDL_FColor col = static_cast<SDL_FColor>(color);

    std::vector<SDL_Vertex> vertices(numVertices);
    vertices[0].position.x = position.x;
    vertices[0].position.y = position.y;
    vertices[0].color = col;

    for (int i = 1; i < numVertices; i++)
    {
        float angle = startRad + (i - 1) * angleStep;
        vertices[i].position.x = position.x + (radius * cos(angle));
        vertices[i].position.y = position.y + (radius * sin(angle));
        vertices[i].color = col;
    }

    return vertices;
}

void drawSemiCircle(Vector2 position, float radius, float direction, Color modulate, Window *window, int numSides)
{
    std::vector<SDL_Vertex> vertices = semiCircleFan(position, radius, direction, modulate, numSides);

    std::vector<int> indices;
    for (int i = 1; i < static_cast<int>(vertices.size()) - 1; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(window->renderer, nullptr, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void drawCircle(Vector2 position, float radius, Color modulate, Window *window, int numSides)
{
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

void drawCapsule(Vector2 center1, Vector2 center2, float radius, Color modulate, Window *window, int numSides)
{
    Vector2 dir = center2 - center1;
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    Vector2 norm = {dir.x / len, dir.y / len};
    Vector2 perp = {-norm.y * radius, norm.x * radius};

    // rectangle between the circles
    const SDL_Vertex verts[4] = {
        {{center1.x + perp.x, center1.y + perp.y}, modulate, {0, 0}},
        {{center1.x - perp.x, center1.y - perp.y}, modulate, {0, 0}},
        {{center2.x - perp.x, center2.y - perp.y}, modulate, {0, 0}},
        {{center2.x + perp.x, center2.y + perp.y}, modulate, {0, 0}},
    };

    const int indices[6] = {0, 1, 2, 2, 3, 0};
    SDL_RenderGeometry(window->renderer, nullptr, verts, 4, indices, 6);

    float angle = atan2(norm.y, norm.x) * 180.0f / 3.141592653f;

    drawSemiCircle(center1, radius, angle + 180, modulate, window, numSides);
    drawSemiCircle(center2, radius, angle, modulate, window, numSides);
}

void drawSegment(Vector2 point1, Vector2 point2, Color modulate, Window *window)
{
    SDL_SetRenderDrawColor(window->renderer, modulate.r, modulate.g, modulate.b, modulate.a);
    SDL_RenderLine(window->renderer, point1.x, point1.y, point2.x, point2.y);
}

void drawPolygon(std::vector<SDL_Vertex> vertices, int count, Color modulate, Window *window)
{
    // fan triangulation from vertex 0
    std::vector<int> indices;
    for (int i = 1; i < count - 1; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(window->renderer, nullptr, vertices.data(), count, indices.data(), static_cast<int>(indices.size()));
}

Vector2 toScreen(Vector2 point, Transform cameraTransform, Transform cameraInverse, Vector2 originOffset, bool screenSpace)
{
    if (!screenSpace)
    {
        point = point - cameraTransform.position;
        point = cameraInverse.transformation * point;
    }
    return point + originOffset;
}