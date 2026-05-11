#pragma once
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"

#include <SDL3/SDL.h>

std::vector<SDL_Vertex> circleFan(Vector2 position, float radius, Color color, int numSides = 20);
void drawCircle(Vector2 position, float radius, Color modulate, Window *window, int numSides = 20);