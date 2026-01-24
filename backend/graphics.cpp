#include "graphics.hpp"
#include "definitions.hpp"
#include "node.hpp"

Graphics::Graphics()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG);

    this->window = SDL_CreateWindow(
        "window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1200,
        900,
        0);

    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

Texture Graphics::loadTexture(const std::string &path)
{
    SDL_Surface *pixels = IMG_Load(path.c_str());
    SDL_Texture *sprite = SDL_CreateTextureFromSurface(renderer, pixels);
    int w = pixels->w;
    int h = pixels->h;
    SDL_FreeSurface(pixels);
    return Texture{sprite, w, h, path};
}

void Graphics::renderSprite(Node &node)
{
    if (!node.currentSprite)
        return;
    Sprite *sprite = node.currentSprite;
    int actual_width = node.transform.scale.x * sprite->texture.width;
    int actual_height = node.transform.scale.y * sprite->texture.height;
    SDL_Rect dst_rect = {node.transform.position.x, node.transform.position.y, actual_width, actual_height};
    SDL_Point center = {node.transform.origin.x * actual_width, node.transform.origin.y * actual_height};
    // SDL_Point center = {(int)actual_width * 0.5, (int)actual_height * 0.5};
    SDL_RenderCopyEx(this->renderer, (SDL_Texture *)sprite->texture.handle, NULL, &dst_rect, node.transform.angle, &center, SDL_FLIP_NONE);
}

void Graphics::preRender()
{
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
}

void Graphics::postRender()
{
    SDL_RenderPresent(this->renderer);
}
