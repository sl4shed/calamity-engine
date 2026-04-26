#include "components.hpp"
#include "node.hpp"
#include "../../services/engine.hpp"
#include "../../services/graphics.hpp"
#include "../../services/services.hpp"
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
Sprite::Sprite(const std::string& texturePath)
{
    texture = Texture(texturePath);
}

void Sprite::initialize()
{
    if (texture.handle != nullptr && sourceRect.size == Vector2{0, 0})
    {
        sourceRect.size = Vector2{static_cast<float>(texture.textureWidth), static_cast<float>(texture.textureHeight)};
    }
}

// shape sprite

ShapeSprite::ShapeSprite() = default;
ShapeSprite::ShapeSprite(const Polygon& shape)
{
    this->shape = shape;
}

// animated sprite

const Texture* AnimatedSprite::getCurrentTexture() const
{
    if (currentTexture.handle == nullptr) return nullptr;
    return &currentTexture;
}

void AnimatedSprite::initialize()
{
    for (auto & [name, anim] : animations)
    {
        if (anim.autoplay)
        {
            // hello. if you have two different animations that are set to autoplay, the one added later into the animations vector will play.
            // this is not my fault. this is purely user error. please reconsider your actions. - Sl4shed, 2026
            play(name);
        }
    }
}

void AnimatedSprite::addAnimation(Animation animation)
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
    if (it == animations.end()) return;

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