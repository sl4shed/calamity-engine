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
#include "physics/definitions.hpp"

#ifdef CALAMITY_VENDORED
#include <SDL3_gfxPrimitives.h>
#else
#include <SDL3_gfx/SDL3_gfxPrimitives.h>
#endif


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

    if (!this->window)
    {
        Logger::error("Failed to create window: {}", SDL_GetError());
    }

    this->renderer = SDL_CreateRenderer(window, nullptr);
    if (!this->renderer)
    {
        Logger::error("Failed to create renderer: {}", SDL_GetError());
    }
    if (!SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), static_cast<SDL_RendererLogicalPresentation>(presentation)))
    {
        Logger::error("Failed to set render logical presentation: {}", SDL_GetError());
    }
    this->textEngine = TTF_CreateSurfaceTextEngine();
    if (!this->textEngine)
    {
        Logger::error("Failed to create text engine: {}", SDL_GetError());
    }
}

void Graphics::resetLogicalPresentation() {
    SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), static_cast<SDL_RendererLogicalPresentation>(presentation));
}

SDL_Texture *Graphics::loadTexture(const std::string &path, TextureScaling scaling) const
{
    SDL_Surface *pixels = IMG_Load(path.c_str());
    if (!pixels) {
        Logger::error("Failed to load image {}: {}", path, SDL_GetError());
        return nullptr;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, pixels);
    if (!tex)
    {
        Logger::error("Failed to create texture {}: {}", path, SDL_GetError());
    }

    if (!SDL_SetTextureScaleMode(tex, static_cast<SDL_ScaleMode>(scaling)))
    {
        Logger::error("Failed to set scale mode for texture {}: {}", path, SDL_GetError());
    }

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
    const auto modulate = static_cast<SDL_FColor>(sprite.modulate);
    auto cameraInverse = cameraTransform.inverse();

    if (const auto* box = dynamic_cast<const BoxShape*>(sprite.shape.get()))
    {
        const Polygon &poly = box->polygon;
        const int count = poly.count;
        if (count < 3) return;

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
    } else if (const auto* circle = dynamic_cast<const CircleShape*>(sprite.shape.get()))
    {
        Vector2 pos = node->globalTransform.position;
        // apply the circle's center offset in local space first
        Vector2 centerOffset = {circle->circle.center.x, circle->circle.center.y};
        centerOffset = node->globalTransform.applyTo(centerOffset) - node->globalTransform.position;
        pos = pos + centerOffset;

        if (!sprite.screenSpace)
        {
            pos = pos - cameraTransform.position;
            pos = cameraInverse.transformation * pos;
        }
        pos = pos + originOffset;

        filledCircleRGBA(renderer, pos.x, pos.y, circle->radius, sprite.modulate.r, sprite.modulate.g, sprite.modulate.b, sprite.modulate.a);
    } else if (const auto* capsule = dynamic_cast<const CapsuleShape*>(sprite.shape.get()))
    {
        Vector2 c1 = node->globalTransform.applyTo(capsule->capsule.center1);
        Vector2 c2 = node->globalTransform.applyTo(capsule->capsule.center2);

        auto toScreen = [&](Vector2 p) -> Vector2 {
            if (!sprite.screenSpace) {
                p = p - cameraTransform.position;
                p = cameraInverse.transformation * p;
            }
            return p + originOffset;
        };

        c1 = toScreen(c1);
        c2 = toScreen(c2);

        float rad = capsule->capsule.radius;

        // direction and perpendicular
        Vector2 dir = {c2.x - c1.x, c2.y - c1.y};
        float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
        Vector2 norm = {dir.x / len, dir.y / len};
        Vector2 perp = {-norm.y * rad, norm.x * rad};

        // rectangle between the circles
        const SDL_Vertex verts[4] = {
            {{c1.x + perp.x, c1.y + perp.y}, modulate, {0, 0}},
            {{c1.x - perp.x, c1.y - perp.y}, modulate, {0, 0}},
            {{c2.x - perp.x, c2.y - perp.y}, modulate, {0, 0}},
            {{c2.x + perp.x, c2.y + perp.y}, modulate, {0, 0}},
        };

        const int indices[6] = {0, 1, 2, 2, 3, 0};
        SDL_RenderGeometry(renderer, nullptr, verts, 4, indices, 6);
        filledCircleRGBA(renderer, c1.x, c1.y, rad, sprite.modulate.r, sprite.modulate.g, sprite.modulate.b, sprite.modulate.a);
        filledCircleRGBA(renderer, c2.x, c2.y, rad, sprite.modulate.r, sprite.modulate.g, sprite.modulate.b, sprite.modulate.a);
    }
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
