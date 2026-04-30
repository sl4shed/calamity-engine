#include "components.hpp"
#include "node.hpp"
#include "../../services/engine.hpp"
#include "../../services/graphics.hpp"
#include "../../services/services.hpp"
#include "../../services/physics/physics.hpp"
#include "../../utils/logger.hpp"
#include <cereal/archives/json.hpp>

Node* Component::getNode() const
{
    return this->node;
}

void Component::setNode(Node* n)
{
    // for now its just this, but I can add checks later to only run this once or maybe i should find a better solution idk
    this->node = n;
}

void Camera::setActive()
{
    Services::engine()->setActiveCamera(this);
    this->active = true;
}

void Camera::initialize()
{
    if (this->active)
    {
        setActive();
    }
}

void Camera::update(float deltaTime) {
    const Vector2 target = getNode()->globalTransform.position;
    
    if (smoothing <= 0.0f)
    {
        globalPos = target;
        return;
    }
    
    const float t = 1.0f - std::exp(-deltaTime / smoothing);
    globalPos = globalPos + (target - globalPos) * t;
}

Transform Camera::getCameraTransform() const 
{
    Transform t = getNode()->globalTransform;
    t.position = globalPos;
    return t;
}

Vector2 Camera::getGlobalPosition()
{
    return globalPos;
}

Vector2 Camera::screenToWorld(const Vector2 screen) const
{
    const auto screenSize = Services::graphics()->screenSize;
    const auto originOffset = screenSize * origin;
    Vector2 v = screen - originOffset;
    v = getNode()->globalTransform.transformation * v;
    v = v + getNode()->globalTransform.position;
    return v;
}

// sprite

Sprite::Sprite() = default;
Sprite::Sprite(const std::string& texturePath, TextureScaling scaling)
{
    texture = Texture(texturePath, scaling);
}

void Sprite::initialize()
{
    if (texture.handle != nullptr && sourceRect.size == Vector2{0, 0})
    {
        sourceRect.size = Vector2{static_cast<float>(texture.textureWidth), static_cast<float>(texture.textureHeight)};
    }
}

// animated sprite

const Texture* AnimatedSprite::getCurrentTexture() const
{
    if (currentTexture.handle == nullptr) return nullptr;
    return &currentTexture;
}

void AnimatedSprite::initialize()
{
    int counter = 0;
    for (auto & [name, anim] : animations)
    {
        if (anim.autoplay)
        {
            counter++;
            // hello. if you have two different animations that are set to autoplay, the one added later into the animations vector will play.
            // this is not my fault. this is purely user error. please reconsider your actions. - Sl4shed, 2026
            play(name);
        }
    }
    if (counter > 1)
    {
        Logger::warn("AnimatedSprite initialized with {} animations set to auto play.", counter);
    }
}

void AnimatedSprite::addAnimation(const Animation& animation)
{
    animations.insert(std::make_pair(animation.name, animation));
}

void AnimatedSprite::removeAnimation(const std::string& name)
{
    animations.erase(name);
}

void AnimatedSprite::play(const std::string& animation)
{
    if (currentAnimation && currentAnimation->name == animation)
    {
        playing = true;
        return;
    }

    const auto it = animations.find(animation);
    if (it == animations.end())
    {
        Logger::warn("No animation found with the name {}.", animation);
        return;
    };

    currentAnimation = std::make_unique<Animation>(it->second);
    changed.fire(currentAnimation->name);

    if (currentAnimation->texturePath != currentTexture.path)
        currentTexture = Texture(currentAnimation->texturePath);

    frame = 0;
    elapsed = 0.0f;
    playing = true;
}

void AnimatedSprite::pause()
{
    playing = false;
    paused.fire(currentAnimation->name);
}

void AnimatedSprite::stop()
{
    playing = false;
    frame = 0;
    elapsed = 0.0f;
    stopped.fire(currentAnimation->name);
    currentAnimation = nullptr;
}

Frame* AnimatedSprite::getCurrentFrame() const
{
    if (!currentAnimation || currentAnimation->frames.empty()) return nullptr;
    return &currentAnimation->frames[frame];
}

Vector2 AnimatedSprite::getCurrentSize() const
{
    if (!currentAnimation) return {0, 0};
    return currentAnimation->size;
}

std::string AnimatedSprite::getCurrentAnimationName() const
{
    if(!currentAnimation) return "";
    return currentAnimation->name;
}

Animation *AnimatedSprite::getCurrentAnimation() const
{
    if(!currentAnimation) return nullptr;
    return currentAnimation.get();
}

void AnimatedSprite::update(float deltaTime)
{
    if (!playing || !currentAnimation) return;

    elapsed += deltaTime;

    if (const float frameDuration = 1.0f / static_cast<float>(currentAnimation->fps); elapsed >= frameDuration)
    {
        elapsed -= frameDuration;
        frame++;

        if (frame >= currentAnimation->frames.size())
        {
            if (currentAnimation->loop)
            {
                looped.fire(currentAnimation->name);
                frame = 0;
                return;
            }
            finished.fire(currentAnimation->name);
            frame = 0;
            playing = false;
        }
    }
}

bool AnimatedSprite::isPlaying() const
{
    return this->playing;
}

// Tilemap

Tilemap::Tilemap(const std::string& texturePath, const TextureScaling scaling, const Vector2 tileSize)
{
    this->texture = Texture(texturePath, scaling);
    this->tileSize = tileSize;
}

void Tilemap::bake()
{
    vertexBuffer.clear();
    indexBuffer.clear();

    const auto atlasW = static_cast<float>(texture.textureWidth);
    const auto atlasH = static_cast<float>(texture.textureHeight);

    for (const auto& tile : tiles)
    {
        const float x = tile.gridPosition.x * tileSize.x;
        const float y = tile.gridPosition.y * tileSize.y;

        const float u0 = tile.sourceRect.position.x / atlasW;
        const float v0 = tile.sourceRect.position.y / atlasH;
        const float u1 = (tile.sourceRect.position.x + tile.sourceRect.size.x) / atlasW;
        const float v1 = (tile.sourceRect.position.y + tile.sourceRect.size.y) / atlasH;

        const auto color = static_cast<SDL_FColor>(tile.modulate);

        const int base = static_cast<int>(vertexBuffer.size());
        vertexBuffer.push_back({{x, y}, color, {u0, v0}});
        vertexBuffer.push_back({{x + tileSize.x, y }, color, {u1, v0}});
        vertexBuffer.push_back({{x + tileSize.x, y + tileSize.y}, color, {u1, v1}});
        vertexBuffer.push_back({{x, y + tileSize.y}, color, {u0, v1}});

        for (const int i : {0, 1, 2, 2, 3, 0}) indexBuffer.push_back(base + i);
    }

    dirty = false;
}

void Tilemap::update()
{
    if (dirty) bake();
}

int Tilemap::addTile(const Tile& tile)
{
    tiles.push_back(tile);
    dirty = true;
    return static_cast<int>(tiles.size()) - 1;
}

void Tilemap::removeTile(int index)
{
    if (index < 0 || index >= static_cast<int>(tiles.size())) return;
    tiles.erase(tiles.begin() + index);
    dirty = true;
}
