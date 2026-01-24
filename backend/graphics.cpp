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

    SDL_Vertex vertices[4];
    vertices[0] = {{-(sprite->texture.width * sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {255, 255, 255, 100}, {0, 0}};
    vertices[1] = {{sprite->texture.width * (1 - sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {255, 255, 255, 255}, {1, 0}};
    vertices[2] = {{sprite->texture.width * (1 - sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {255, 255, 255, 255}, {1, 1}};
    vertices[3] = {{-(sprite->texture.width * sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {255, 255, 255, 255}, {0, 1}};

    for (int i = 0; i < 4; i++)
    {
        Vector2 pos = {vertices[i].position.x, vertices[i].position.y};
        pos = node.globalTransform.transformation * pos;
        vertices[i].position.x = pos.x + node.globalTransform.position.x;
        vertices[i].position.y = pos.y + node.globalTransform.position.y;

        Vector2 texturePos = {vertices[i].tex_coord.x, vertices[i].tex_coord.y};
        texturePos = sprite->sourceTransform.transformation * texturePos;
        vertices[i].tex_coord.x = texturePos.x + sprite->sourceTransform.position.x;
        vertices[i].tex_coord.y = texturePos.y + sprite->sourceTransform.position.y;
    }

    int indices[6] = {0, 1, 2, 2, 3, 0};

    int status = SDL_RenderGeometry(this->renderer, (SDL_Texture *)sprite->texture.handle, vertices, 4, indices, 6);
    // printf("RenderGeometry status: %d\n", status);
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
