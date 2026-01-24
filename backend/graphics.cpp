#include <SDL3/SDL.h>
#include "graphics.hpp"
#include "definitions.hpp"
#include "node.hpp"
#include "engine.hpp"

Graphics::Graphics(Vector2 s)
    : screenSize(s)
{
    screenSize = s;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

    this->window = SDL_CreateWindow(
        "window",
        screenSize.x,
        screenSize.y,
        0);

    this->renderer = SDL_CreateRenderer(window, NULL);
}

Texture Graphics::loadTexture(const std::string &path)
{
    SDL_Surface *pixels = IMG_Load(path.c_str());
    SDL_Texture *sprite = SDL_CreateTextureFromSurface(renderer, pixels);
    int w = pixels->w;
    int h = pixels->h;
    SDL_DestroySurface(pixels);
    return Texture{sprite, w, h, path};
}

void Graphics::renderSprite(Node &node, Engine *engine)
{
    if (!node.currentSprite)
        return;

    Camera *activeCamera = engine->getActiveCamera();
    Transform cameraTransform = activeCamera->getNode()->globalTransform;
    Sprite *sprite = node.currentSprite;
    SDL_Vertex vertices[4];
    vertices[0] = {{-(sprite->texture.width * sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {1, 1, 1, 1}, {0, 0}};
    vertices[1] = {{sprite->texture.width * (1 - sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {1, 1, 1, 1}, {1, 0}};
    vertices[2] = {{sprite->texture.width * (1 - sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {1, 1, 1, 1}, {1, 1}};
    vertices[3] = {{-(sprite->texture.width * sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {1, 1, 1, 1}, {0, 1}};

    for (int i = 0; i < 4; i++)
    {
        // for camera origin copy transform and offset by screen size
        Transform newCameraTransform = cameraTransform;
        newCameraTransform.position = newCameraTransform.position - Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};

        Vector2 pos = {vertices[i].position.x, vertices[i].position.y};
        pos = node.globalTransform.applyTo(pos);
        pos = cameraTransform.inverse().applyTo(pos);
        // pos = newCameraTransform.inverse().applyTo(pos);
        // Transform inverse = newCameraTransform.inverse();

        vertices[i].position.x = pos.x;
        vertices[i].position.y = pos.y;

        Vector2 texturePos = {vertices[i].tex_coord.x, vertices[i].tex_coord.y};
        texturePos = sprite->sourceTransform.applyTo(texturePos);
        vertices[i].tex_coord.x = texturePos.x;
        vertices[i].tex_coord.y = texturePos.y;
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
