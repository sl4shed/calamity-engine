#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "graphics.hpp"
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../utils/logger.hpp"
#include "../core/ui/definitions.hpp"
#include "engine.hpp"
#include "../core/ui/label.hpp"

Graphics::Graphics(Vector2 s, std::string t, RenderLogicalPresentation p, Color cc, WindowFlags flags)
    : screenSize(s)
{
    presentation = p;
    clearColor = cc;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
    TTF_Init();

#ifdef EMSCRIPTEN
    SDL_AddGamepadMappingsFromFile("/calamity/gamecontrollerdb.txt");
#else
    SDL_AddGamepadMappingsFromFile("./calamity/gamecontrollerdb.txt");
#endif

    this->window = SDL_CreateWindow(
        t.c_str(),
        screenSize.x,
        screenSize.y,
        (SDL_WindowFlags)flags
    );

    this->renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderLogicalPresentation(renderer, screenSize.x, screenSize.y, (SDL_RendererLogicalPresentation)presentation);
    this->textEngine = TTF_CreateSurfaceTextEngine();
}

void Graphics::resetLogicalPresentation() {
    SDL_SetRenderLogicalPresentation(renderer, screenSize.x, screenSize.y, (SDL_RendererLogicalPresentation)presentation);
}

SDL_Texture *Graphics::loadTexture(const std::string &path)
{
    SDL_Surface *pixels = IMG_Load(path.c_str());
    if (!pixels) {
        Logger::error("Failed to load texture '{}': {}", path, SDL_GetError());
        return nullptr;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, pixels);
    SDL_DestroySurface(pixels);
    return tex;
}

SDL_Renderer* Graphics::getRenderer()
{
    return renderer;
}

void Graphics::renderSprite(Node &node, Engine *engine)
{
    if (!node.currentSprite) return;
    if (!node.currentSprite->texture.handle) return;

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
        if (!sprite->screenSpace)
        {
            pos = pos - cameraTransform.position;
            Transform cameraInverse = cameraTransform.inverse();
            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;
        vertices[i].position.x = pos.x;
        vertices[i].position.y = pos.y;

        Transform sourceTransform = sprite->sourceTransform;
        Vector2 texturePos = {vertices[i].tex_coord.x, vertices[i].tex_coord.y};
        texturePos = texturePos * sourceTransform.getScale() + Vector2{sourceTransform.position.x / (float)sprite->texture.width, sourceTransform.position.y / (float)sprite->texture.height};
        vertices[i].tex_coord.x = texturePos.x;
        vertices[i].tex_coord.y = texturePos.y;
    }

    int indices[6] = {0, 1, 2, 2, 3, 0};

    SDL_RenderGeometry(this->renderer, sprite->texture.handle, vertices, 4, indices, 6);
}

void Graphics::renderPolygonSprite(Node &node, Engine *engine) {
    std::shared_ptr<ShapeSprite> polySprite = node.getComponent<ShapeSprite>();
    if (!polySprite) return;

    Camera *activeCamera = engine->getActiveCamera();
    Transform cameraTransform = activeCamera->getNode()->globalTransform;
    Vector2 originOffset = {screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};

    Polygon &poly = polySprite->shape;
    int count = poly.count;
    if (count < 3) return;

    SDL_FColor col = {polySprite->color.r / 255.0f, polySprite->color.g / 255.0f, polySprite->color.b / 255.0f, polySprite->color.a / 255.0f};
    std::vector<SDL_Vertex> vertices(count);
    for (int i = 0; i < count; i++) {
        Vector2 pos = poly.vertices[i];
        pos = node.globalTransform.applyTo(pos);
        if (!polySprite->screenSpace)
        {
            pos = pos - cameraTransform.position;
            pos = cameraTransform.inverse().transformation * pos;
        }
        pos = pos + originOffset;
        vertices[i] = {{pos.x, pos.y}, col, {0, 0}};
    }

    // fan triangulation from vertex 0
    std::vector<int> indices;
    for (int i = 1; i < count - 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(renderer, nullptr, vertices.data(), count, indices.data(), indices.size());
}

void Graphics::renderLabel(Label *label) {
    if(!label->getNode()) return;

    // hopefully this works
    SDL_Texture *texture = label->getTexture();
    if(!texture) return;

    Node *lnode = label->getNode();

    Camera *activeCamera = Services::engine()->getActiveCamera();
    Transform cameraTransform = activeCamera->getNode()->globalTransform;
    SDL_Vertex vertices[4];

    float w;
    float h;
    SDL_GetTextureSize(texture, &w, &h);
    vertices[0] = {{-(w * label->origin.x), -(h * label->origin.y)}, {1, 1, 1, 1}, {0, 0}};
    vertices[1] = {{w * (1 - label->origin.x), -(h * label->origin.y)}, {1, 1, 1, 1}, {1, 0}};
    vertices[2] = {{w * (1 - label->origin.x), h * (1 - label->origin.y)}, {1, 1, 1, 1}, {1, 1}};
    vertices[3] = {{-(w * label->origin.x), h * (1 - label->origin.y)}, {1, 1, 1, 1}, {0, 1}};

    Vector2 originOffset = Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};
    Transform cameraInverse = cameraTransform.inverse();

    for (int i = 0; i < 4; i++)
    {
        Vector2 pos = {vertices[i].position.x, vertices[i].position.y};
        pos = lnode->globalTransform.applyTo(pos);

        if (!label->screenSpace) {
            pos = pos - cameraTransform.position;
            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;

        vertices[i].position.x = pos.x;
        vertices[i].position.y = pos.y;

        Vector2 texturePos = {vertices[i].tex_coord.x, vertices[i].tex_coord.y};
        vertices[i].tex_coord.x = texturePos.x;
        vertices[i].tex_coord.y = texturePos.y;
    }

    int indices[6] = {0, 1, 2, 2, 3, 0};

    SDL_RenderGeometry(this->renderer, texture, vertices, 4, indices, 6);
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
