#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "definitions.hpp"
#include "components.hpp"

class Graphics
{
public:
    Graphics(Vector2 screenSize = {480, 272});
    Texture loadTexture(const std::string &path);
    void renderSprite(Node &node, Engine *engine);
    void preRender();
    void postRender();
    Vector2 screenSize;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};