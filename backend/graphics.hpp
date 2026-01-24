#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "definitions.hpp"
#include "components.hpp"

class Graphics
{
public:
    Graphics();
    Texture loadTexture(const std::string &path);
    void renderSprite(Node &node);
    void preRender();
    void postRender();
    SDL_Window *window;
    SDL_Renderer *renderer;
};