#pragma once
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../services/input/keycode.hpp"
#include "../../services/input/input.hpp"
#include "../ui/definitions.hpp"
#include "backend/core/signal.hpp"
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

class Node; // Forward declaration'

/**
 * # Component class
 * Kinda self-explanatory. You have the ``update()`` and ``initialize()`` methods which should be ran by the parent node correspondingly.
 *
 * For serialization of any class that is based on Component, you need to implement the ``save()`` and ``load()`` methods, which are used by cereal for serialization.
 */
struct Component
{
    virtual ~Component() = default;
    virtual void update(float deltaTime) {};
    virtual void initialize() {};
    virtual void physicsUpdate() {};
    virtual void input(InputEvent& event) {};
    virtual void exit() {};

    Node *getNode() const;
    void setNode(Node *n);

    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

private:
    Node *node;
};

/**
 * # Sprite component
 * The sprite component renders a texture.
 *
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> spriteNode = std::make_shared<Node>();
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
 * sprite->texture = Texture("path/to/texture.png");
 * spriteNode->addComponent(sprite);
 * ```
 *
 * Be sure to also check out the atlas example in the examples folder!
 */
class Sprite : public Component
{
public:
    Sprite();
    explicit Sprite(const std::string& texturePath, TextureScaling scaling = TextureScaling::NEAREST);

    void initialize() override;

    Vector2 origin = {0.5f, 0.5f};
    Texture texture;
    Vector2 tileSize = {0, 0}; // this means no tiling

    Rect sourceRect;
    bool visible = true;
    //int zIndex = 1;
    bool screenSpace = false;
    Color modulate = Color::WHITE;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceRect), CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceRect), CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate));
    }
};

class AnimatedSprite : public Component
{
public:
    AnimatedSprite() {};

    void initialize() override;
    void update(float deltaTime) override;

    void addAnimation(const Animation& animation);
    void removeAnimation(const std::string& name);

    void play(const std::string& animation);
    void stop();
    void pause();

    Signal<std::string> changed;
    Signal<std::string> finished;
    Signal<std::string> looped;
    Signal<std::string> stopped;
    Signal<std::string> paused;

    const Texture* getCurrentTexture() const;
    Vector2 getCurrentSize() const;
    bool isPlaying() const;
    Frame *getCurrentFrame() const;

    std::map<std::string, Animation> animations;
    bool visible = true;
    //int zIndex = 1; I'm not implementing this rn
    bool screenSpace = false;

    template <class Archive>
    void save(Archive &ar) const
    {
        std::string currentAnimName = currentAnimation ? currentAnimation->name : "";
        ar(CEREAL_NVP(animations), CEREAL_NVP(visible), CEREAL_NVP(screenSpace),
           CEREAL_NVP(frame), CEREAL_NVP(playing), currentAnimName);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        std::string currentAnimName;
        ar(CEREAL_NVP(animations), CEREAL_NVP(visible), CEREAL_NVP(screenSpace),
           CEREAL_NVP(frame), CEREAL_NVP(playing), currentAnimName);

        // reconstruct runtime state
        if (!currentAnimName.empty())
            play(currentAnimName);
    }
private:
    float elapsed = 0.0f;
    int frame = 0;
    std::unique_ptr<Animation> currentAnimation = nullptr;
    Texture currentTexture;
    bool playing = false;
};

class Tilemap : public Component
{
public:
    Tilemap() = default;
    Tilemap(const std::string& texturePath, TextureScaling scaling, Vector2 tileSize);

    bool visible = true;
    Texture texture;
    Vector2 tileSize;
    std::vector<Tile> tiles;

    // sdl bullshit
    std::vector<SDL_Vertex> vertexBuffer;
    std::vector<int> indexBuffer;

    int addTile(const Tile& tile);
    void removeTile(int index);

    template <typename... Args>
    void addTiles(Args... tiles)
    {
        (this->tiles.push_back(tiles), ...);
        dirty = true;
    }

    void update();
    void initialize() { bake(); };
    void bake();

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(tileSize), CEREAL_NVP(tiles));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(tileSize), CEREAL_NVP(tiles));
        bake();
    }
private:
    bool dirty = false;
};

/**
 * # Script component
 * A base class for all scripts attached to nodes to inherit from. It provides the necessary api's like update, initialize, physicsUpdate, input and exit.
 *
 * To create a script and attach it to a node, create a header file somewhere like `scripts/ExampleScript.hpp`:
 * ```cpp
 *
 * #pragma once
 *
 * // include everything you want here
 * #include <cereal/types/polymorphic.hpp>
 * #include <cereal/archives/json.hpp>
 *
 * class ExampleScript : public Script
 * {
 *
 * public:
 *     // serialization functions to make cereal happy.
 *     // you can leave these empty if you don't need to serialize any data in your script, but they need to be here for cereal to work.
 *     // to serialize something, call ``ar(variable1, variable2, variable3)`` in both functions, but make sure to call it in the same order in both functions.
 *     template <class Archive>
 *     void save(Archive &ar) const {};
 *
 *     template <class Archive>
 *     void load(Archive &ar) {};
 *
 *     void initialize()
 *     {
 *         // initialize logic here
 *     }
 *
 *     void update(float deltaTime)
 *     {
 *         // update logic here
 *     }
 * };
 *
 * // you also have to do this for your game to compile
 * CEREAL_REGISTER_TYPE(ExampleScript);
 * CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, ExampleScript);
 * ```
 *
 * and do this to your node:
 * ```cpp
 * std::shared_ptr<Node> exampleNode = std::make_shared<Node>();
 * exampleNode->addComponent(std::make_shared<ExampleScript>());
 * ```
 */
class Script : public Component
{
public:
    virtual void update(float deltaTime) {};
    virtual void initialize() {};
    virtual void physicsUpdate() {};
    virtual void exit() {};

    template <class Archive>
    void save(Archive &ar) const {};

    template <class Archive>
    void load(Archive &ar) {};
};

/**
 * # Camera component
 */
class Camera : public Component
{
public:
    void setActive();
    bool active = true;
    Vector2 origin = {0.5f, 0.5f};

    void initialize() override;

    Vector2 screenToWorld(Vector2 screen) const;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(active), CEREAL_NVP(origin));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(active), CEREAL_NVP(origin));
    }
};

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

CEREAL_REGISTER_TYPE(Sprite)
CEREAL_REGISTER_TYPE(Camera)
CEREAL_REGISTER_TYPE(Script)
CEREAL_REGISTER_TYPE(AnimatedSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Sprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, AnimatedSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Script)