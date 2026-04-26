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

Graphics::Graphics(Vector2 _screenSize, const std::string& _title, RenderLogicalPresentation _presentation, Color _clearColor, WindowFlags _flags)
    : screenSize(_screenSize), clearColor(_clearColor), presentation(_presentation)
{
    presentation = _presentation;
    clearColor = _clearColor;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
    TTF_Init();

#ifdef EMSCRIPTEN
    SDL_AddGamepadMappingsFromFile("/calamity/gamecontrollerdb.txt");
#else
    SDL_AddGamepadMappingsFromFile("./calamity/gamecontrollerdb.txt");
#endif

    this->window = SDL_CreateWindow(
        _title.c_str(),
        static_cast<int>(screenSize.x),
        static_cast<int>(screenSize.y),
        static_cast<SDL_WindowFlags>(_flags)
    );

    this->renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), static_cast<SDL_RendererLogicalPresentation>(presentation));
    this->textEngine = TTF_CreateSurfaceTextEngine();
}

void Graphics::resetLogicalPresentation() {
    SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), static_cast<SDL_RendererLogicalPresentation>(presentation));
}

SDL_Texture *Graphics::loadTexture(const std::string &path) const
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

SDL_Renderer* Graphics::getRenderer() const
{
    return renderer;
}

void Graphics::renderComponent(const Sprite &sprite) const
{
    const Node *node = sprite.getNode();
    if (!node) return;
    if (!sprite.texture.handle) return;

    const Camera *activeCamera = Services::engine()->getActiveCamera();
    const Transform cameraTransform = activeCamera->getNode()->globalTransform;
    SDL_Vertex vertices[4];
    const auto modulate = static_cast<SDL_FColor>(sprite.modulate);
    vertices[0] = {{-(sprite.texture.width * sprite.origin.x), -(sprite.texture.height * sprite.origin.y)}, modulate, {0, 0}};
    vertices[1] = {{sprite.texture.width * (1 - sprite.origin.x), -(sprite.texture.height * sprite.origin.y)}, modulate, {1, 0}};
    vertices[2] = {{sprite.texture.width * (1 - sprite.origin.x), sprite.texture.height * (1 - sprite.origin.y)}, modulate, {1, 1}};
    vertices[3] = {{-(sprite.texture.width * sprite.origin.x), sprite.texture.height * (1 - sprite.origin.y)}, modulate, {0, 1}};

    const auto originOffset = Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};
    auto cameraInverse = cameraTransform.inverse();
    const Rect sourceRect = sprite.sourceRect;
    for (auto & vertice : vertices)
    {
        Vector2 pos = {vertice.position.x, vertice.position.y};
        pos = node->globalTransform.applyTo(pos);

        // translate to camera space or wtv
        if (!sprite.screenSpace)
        {
            pos = pos - cameraTransform.position;
            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;
        vertice.position.x = pos.x;
        vertice.position.y = pos.y;

        Vector2 texturePos = {vertice.tex_coord.x, vertice.tex_coord.y};
        texturePos = texturePos * (sourceRect.size / Vector2{static_cast<float>(sprite.texture.textureWidth), static_cast<float>(sprite.texture.textureHeight)})
           + Vector2{sourceRect.position.x / static_cast<float>(sprite.texture.textureWidth),
                     sourceRect.position.y / static_cast<float>(sprite.texture.textureHeight)};
        vertice.tex_coord.x = texturePos.x;
        vertice.tex_coord.y = texturePos.y;
    }

    const int indices[6] = {0, 1, 2, 2, 3, 0};
    SDL_RenderGeometry(this->renderer, sprite.texture.handle, vertices, 4, indices, 6);
}

void Graphics::renderComponent(const ShapeSprite &sprite) const
{
    const Node *node = sprite.getNode();
    if (!node) return;

    const Camera *activeCamera = Services::engine()->getActiveCamera();
    const Transform cameraTransform = activeCamera->getNode()->globalTransform;
    const Vector2 originOffset = {screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};

    const Polygon &poly = sprite.shape;
    const int count = poly.count;
    if (count < 3) return;

    const auto modulate = static_cast<SDL_FColor>(sprite.modulate);
    auto cameraInverse = cameraTransform.inverse();
    std::vector<SDL_Vertex> vertices(count);
    for (int i = 0; i < count; i++) {
        Vector2 pos = poly.vertices[i];
        pos = node->globalTransform.applyTo(pos);
        if (!sprite.screenSpace)
        {
            pos = pos - cameraTransform.position;
            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;

        vertices[i] = {{pos.x, pos.y}, modulate, {0, 0}};
    }

    // fan triangulation from vertex 0
    std::vector<int> indices;
    for (int i = 1; i < count - 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(renderer, nullptr, vertices.data(), count, indices.data(), static_cast<int>(indices.size()));
}

void Graphics::renderComponent(const Label &label) const
{
    SDL_Texture *texture = label.getTexture();
    const Node *lnode = label.getNode();
    if(!lnode) return;
    if(!texture) return;

    const Camera *activeCamera = Services::engine()->getActiveCamera();
    const Transform cameraTransform = activeCamera->getNode()->globalTransform;
    SDL_Vertex vertices[4];

    float w;
    float h;
    SDL_GetTextureSize(texture, &w, &h);

    vertices[0] = {{-(w * label.origin.x), -(h * label.origin.y)}, {1, 1, 1, 1}, {0, 0}};
    vertices[1] = {{w * (1 - label.origin.x), -(h * label.origin.y)}, {1, 1, 1, 1}, {1, 0}};
    vertices[2] = {{w * (1 - label.origin.x), h * (1 - label.origin.y)}, {1, 1, 1, 1}, {1, 1}};
    vertices[3] = {{-(w * label.origin.x), h * (1 - label.origin.y)}, {1, 1, 1, 1}, {0, 1}};

    const auto originOffset = Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};
    auto [position, transformation] = cameraTransform.inverse();

    for (auto & vertice : vertices)
    {
        Vector2 pos = {vertice.position.x, vertice.position.y};
        pos = lnode->globalTransform.applyTo(pos);

        if (!label.screenSpace) {
            pos = pos - cameraTransform.position;
            pos = transformation * pos;
        }
        pos = pos + originOffset;

        vertice.position.x = pos.x;
        vertice.position.y = pos.y;

        const Vector2 texturePos = {vertice.tex_coord.x, vertice.tex_coord.y};
        vertice.tex_coord.x = texturePos.x;
        vertice.tex_coord.y = texturePos.y;
    }

    const int indices[6] = {0, 1, 2, 2, 3, 0};
    SDL_RenderGeometry(this->renderer, texture, vertices, 4, indices, 6);
}

void Graphics::renderComponent(const AnimatedSprite &sprite) const
{
    if (!sprite.isPlaying()) return;

    const Node *node = sprite.getNode();
    const auto frame = sprite.getCurrentFrame();
    const auto texture = sprite.getCurrentTexture();
    const Vector2 renderSize = sprite.getCurrentSize();
    if (!node) return;
    if (!frame) return;
    if (!texture) return;

    const Camera *activeCamera = Services::engine()->getActiveCamera();
    const Transform cameraTransform = activeCamera->getNode()->globalTransform;
    const Rect sourceRect = frame->rect;

    SDL_Vertex vertices[4];
    const auto modulate = static_cast<SDL_FColor>(frame->modulate);
    vertices[0] = {{-(renderSize.x * frame->origin.x), -(renderSize.y * frame->origin.y)}, modulate, {0, 0}};
    vertices[1] = {{renderSize.x * (1 - frame->origin.x), -(renderSize.y * frame->origin.y)}, modulate, {1, 0}};
    vertices[2] = {{renderSize.x * (1 - frame->origin.x), renderSize.y * (1 - frame->origin.y)}, modulate, {1, 1}};
    vertices[3] = {{-(renderSize.x * frame->origin.x), renderSize.y * (1 - frame->origin.y)}, modulate, {0, 1}};

    const auto originOffset = Vector2{screenSize.x * activeCamera->origin.x, screenSize.y * activeCamera->origin.y};

    auto cameraInverse = cameraTransform.inverse();
    for (auto & vertice : vertices)
    {
        Vector2 pos = {vertice.position.x, vertice.position.y};
        pos = node->globalTransform.applyTo(pos);

        if (!sprite.screenSpace)
        {
            pos = pos - cameraTransform.position;

            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;
        vertice.position.x = pos.x;
        vertice.position.y = pos.y;

        Vector2 texturePos = {vertice.tex_coord.x, vertice.tex_coord.y};
        texturePos = texturePos * (sourceRect.size / Vector2{static_cast<float>(texture->textureWidth), static_cast<float>(texture->textureHeight)})
                   + Vector2{sourceRect.position.x / static_cast<float>(texture->textureWidth),
                             sourceRect.position.y / static_cast<float>(texture->textureHeight)};
        vertice.tex_coord.x = texturePos.x;
        vertice.tex_coord.y = texturePos.y;
    }

    const int indices[6] = {0, 1, 2, 2, 3, 0};
    SDL_RenderGeometry(this->renderer, texture->handle, vertices, 4, indices, 6);
}

TTF_TextEngine *Graphics::getTextEngine() const
{
    return textEngine;
}

void Graphics::preRender() const
{
    SDL_SetRenderDrawColor(this->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(this->renderer);
}

void Graphics::postRender() const
{
    SDL_RenderPresent(this->renderer);
}
