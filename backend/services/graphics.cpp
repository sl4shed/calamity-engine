#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "graphics.hpp"
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../utils/logger.hpp"
#include "engine.hpp"

Graphics::Graphics(Vector2 s, std::string title, RenderLogicalPresentation p, Color cc)
    : screenSize(s)
{
    screenSize = s;
    presentation = p;
    clearColor = cc;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
    TTF_Init();
    SDL_AddGamepadMappingsFromFile("./calamity/gamecontrollerdb.txt");

    this->window = SDL_CreateWindow(
        title.c_str(),
        screenSize.x,
        screenSize.y,
        SDL_WINDOW_RESIZABLE);
    
    SDL_SetRenderLogicalPresentation(renderer, screenSize.x, screenSize.y, (SDL_RendererLogicalPresentation)presentation);

    this->renderer = SDL_CreateRenderer(window, NULL);
    this->textEngine = TTF_CreateRendererTextEngine(this->renderer);
}

void Graphics::resetLogicalPresentation() {
    SDL_SetRenderLogicalPresentation(renderer, screenSize.x, screenSize.y, (SDL_RendererLogicalPresentation)presentation);
}

SDL_Texture *Graphics::loadTexture(const std::string &path)
{
    SDL_Surface *pixels = IMG_Load(path.c_str());
    return SDL_CreateTextureFromSurface(renderer, pixels);
}

void Graphics::renderSprite(Node &node, Engine *engine)
{
    if (!node.currentSprite) return;

    Camera *activeCamera = engine->getActiveCamera();
    Transform cameraTransform = activeCamera->getNode()->globalTransform;
    Sprite *sprite = node.currentSprite;
    SDL_Vertex vertices[4];
    vertices[0] = {{-(sprite->texture.width * sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {1, 1, 1, 1}, {0, 0}};
    vertices[1] = {{sprite->texture.width * (1 - sprite->origin.x), -(sprite->texture.height * sprite->origin.y)}, {1, 1, 1, 1}, {1, 0}};
    vertices[2] = {{sprite->texture.width * (1 - sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {1, 1, 1, 1}, {1, 1}};
    vertices[3] = {{-(sprite->texture.width * sprite->origin.x), sprite->texture.height * (1 - sprite->origin.y)}, {1, 1, 1, 1}, {0, 1}};

    Vector2 originOffset = Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};

    for (int i = 0; i < 4; i++)
    {
        Vector2 pos = {vertices[i].position.x, vertices[i].position.y};
        pos = node.globalTransform.applyTo(pos);

        // translate to camera space or wtv
        pos = pos - cameraTransform.position;
        Transform cameraInverse = cameraTransform.inverse();
        pos = cameraInverse.transformation * pos;
        pos = pos + originOffset;
        vertices[i].position.x = pos.x;
        vertices[i].position.y = pos.y;

        Vector2 texturePos = {vertices[i].tex_coord.x, vertices[i].tex_coord.y};
        texturePos = sprite->sourceTransform.applyTo(texturePos);
        vertices[i].tex_coord.x = texturePos.x;
        vertices[i].tex_coord.y = texturePos.y;
    }

    int indices[6] = {0, 1, 2, 2, 3, 0};

    int status = SDL_RenderGeometry(this->renderer, sprite->texture.handle, vertices, 4, indices, 6);
    // printf("RenderGeometry status: %d\n", status);
}

void Graphics::renderLabel(Label *label) {
    if(!label->getNode()) return;
    
    if(!label->wrap) {
        TTF_SetTextWrapWidth(label->getHandle(), 0);
    }

    Vector2 pos = label->getNode()->transform.position;
    TTF_DrawRendererText(label->getHandle(), pos.x, pos.y);
}

TTF_TextEngine *Graphics::getTextEngine() {
    return textEngine;
}

void Graphics::preRender()
{
    SDL_SetRenderDrawColor(this->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(this->renderer);
}

void Graphics::postRender()
{
    SDL_RenderPresent(this->renderer);
}
