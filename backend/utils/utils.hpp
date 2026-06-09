#pragma once
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"
#include "../services/physics/definitions.hpp"

#include <SDL3/SDL.h>

std::vector<SDL_Vertex> circleFan(Vector2 position, float radius, Color color, int numSides = 20);
void drawCircle(Vector2 position, float radius, Color modulate, Window *window, int numSides = 20);
void drawCapsule(Vector2 center1, Vector2 center2, float radius, Color modulate, Window *window, int numSides = 20);
void drawPolygon(std::vector<SDL_Vertex> vertices, int count, Color modulate, Window *window);
void drawRoundedBox(Vector2 position, Vector2 size, float radius, Color modulate, Window *window, int numSides = 20);

Vector2 toScreen(Vector2 point, Transform cameraTransform, Transform cameraInverse, Vector2 originOffset, bool screenSpace);
