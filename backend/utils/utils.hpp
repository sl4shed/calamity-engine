#pragma once
#include "../core/definitions.hpp"
#include "../core/node/components.hpp"
#include "../services/physics/definitions.hpp"

#include <SDL3/SDL.h>

std::vector<SDL_Vertex> circleFan(Vector2 position, float radius, Color color, int numSides = 20);
std::vector<SDL_Vertex> semiCircleFan(Vector2 position, float radius, float direction, Color color, int numSides = 10);

void drawSemiCircle(Vector2 position, float radius, float direction, Color modulate, Window *window, int numSides = 10);
void drawCircle(Vector2 position, float radius, Color modulate, Window *window, int numSides = 20);
void drawCapsule(Vector2 center1, Vector2 center2, float radius, Color modulate, Window *window, int numSides = 20);
void drawPolygon(std::vector<SDL_Vertex> vertices, int count, Color modulate, Window *window);
void drawSegment(Vector2 point1, Vector2 point2, Color modulate, Window *window);

Vector2 toScreen(Vector2 point, Transform cameraTransform, Transform cameraInverse, Vector2 originOffset, bool screenSpace);
