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
 * Base class for any component that can be attached to a Node. It exposes several virtual functiosn that are called across any components lifetime, such as `update(float deltaTime)`, `initialize()`, `input(InputEvent &event)`, `physicsUpdate()` and `exit()`.
 *
 * For serialization of any class that is based on Component, you need to implement the ``save()`` and ``load()`` methods, which are used by [cereal](https://uscilab.github.io/cereal/) for serialization and deserialization.
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
 * The sprite component renders a texture to the screen.
 *
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> spriteNode = std::make_shared<Node>("spriteNode");
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://path/to/texture.png");
 * spriteNode->addComponent(sprite);
 * ```
 * 
 * ## Properties and usages
 * 
 * You can also modify it's origin:
 * ```cpp
 * sprite->origin = {0.0f, 0.0f} // instead of the center of the sprite being at the position of the node, now the top left will be at the position of the node.
 * ```
 * 
 * The sprites Texture also has a width and height attribute. These define the size of the final rendered texture on the screen.
 * ```cpp 
 * sprite->texture.width = 64;
 * sprite->texture.height = 64;
 * ```
 * 
 * When you define the sprite, you can also define the TextureScaling mode:
 * ```cpp
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://path/to/texture.png", TextureScaling::PIXELART);
 * ```
 * 
 * You can also define the sprites source rectangle in a texture atlas. This allows you to pack a bunch of textures in one image which you can load all at once!
 * ```cpp
 * sprite->sourceRect = Rect{Vector2{0.0f, 0.0f}, Vector2{16.0f, 16.0f}};
 * ```
 * 
 * Sprites also have a modulate property, which allows you to modify their color when theyre rendered:
 * ```cpp
 * sprite->modulate = Color::BLUE; // The sprite will now become blue
 * ```
 * 
 * You can also make sprites render using screen space positioning. This can be used for UI elements and things like that:
 * ```cpp
 * sprite->screenSpace = true; // Now, the position of the sprite will directly translate to screen coordinates!
 * ```
 * 
 * Sprites also define `flipH` and `flipV` variables. You can use these when animating a sprite, for example.
 * ```cpp
 * sprite->flipV = true; // The sprite will now render vertically flipped!
 * ```
 *
 * Make sure to also check out the [atlas example](https://calamity.sl4shed.xyz/example-atlas)!
 */
class Sprite : public Component
{
public:
    Sprite();
    explicit Sprite(const std::string& texturePath, TextureScaling scaling = TextureScaling::NEAREST);

    void initialize() override;

    Vector2 origin = {0.5f, 0.5f};
    Texture texture;

    Rect sourceRect;
    bool visible = true;
    //int zIndex = 1;
    bool screenSpace = false;
    Color modulate = Color::WHITE;

    bool flipH = false;
    bool flipV = false;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceRect), CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate), CEREAL_NVP(flipH), CEREAL_NVP(flipV));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceRect), CEREAL_NVP(visible), CEREAL_NVP(texture), CEREAL_NVP(screenSpace), CEREAL_NVP(modulate), CEREAL_NVP(flipH), CEREAL_NVP(flipV));
    }
};

/**
 * # Animated Sprite 
 * The AnimatedSprite class allows you to define \ref Animation "Animations" composed of \ref Frame "Frames" which will get rendered on screen.
 * 
 * Example usage:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>("myNode");
 * std::shared_ptr<AnimatedSprite> sprite = std::make_shared<AnimatedSprite>();
 * // arguments: animation name, FPS, size, loop and autoplay
 * Animation anim = Animation("test", 2, Vector2{15, 15}, true, false);
 * anim.texturePath = "res://assets/frames.png";
 * anim.textureScaling = TextureScaling::PIXELART;
 * 
 * // append several frames to the sprite
 * anim.addFrames(
 *     // This adds a frame, with a sourceRect, a centered origin and the modulate color of blue.
 *     Frame({{0,   0}, {32, 32}}, {0.5, 0.5}, Color::BLUE),
 *     Frame({{32,  0}, {32, 32}}),
 *     Frame({{64,  0}, {32, 32}}),
 *     Frame({{64,  0}, {32, 32}}),
 *     Frame({{64,  0}, {32, 32}}),
 *     Frame({{64,  0}, {32, 32}}),
 *     Frame({{96,  0}, {32, 32}}),
 *     Frame({{128, 0}, {32, 32}})
 * );
 * 
 * sprite->addAnimation(anim);
 * node->addComponent(sprite);
 * 
 * sprite->play("test"); // The animation will now play and loop... forever!
 * ```
 * 
 * ## Properties and usages
 * 
 * You can start, stop and pause animations:
 * ```cpp
 * sprite->play("test");
 * 
 * sprite->pause(); // the animation is now paused.
 * sprite->play("test"); // you can resume it by firing play with the same animation name (in this case, "test")
 * 
 * sprite->stop();
 * ```
 * 
 * AnimatedSprites also have a bunch of signals that you can register to callbacks. For example, you can register a callback when the animation is finished!
 * ```cpp
 * sprite->finished.connect([]() {
 *     Logger::info("Animation finished playing!");
 * });
 * ```
 * 
 * And, just like regular \ref Sprite "Sprites", AnimatedSprites have the `flipV` and `flipH` attributes. This is useful when animating the sprite!
 * ```cpp
 * sprite->flipV = true; // The sprite will now render vertically flipped!
 * ```
 * 
 * You can also make AnimatedSprites render using screen space positioning. This can be used for UI elements and things like that:
 * ```cpp
 * sprite->screenSpace = true; // Now, the position of the sprite will directly translate to screen coordinates!
 * ```
 * 
 * Make sure to also check out the [animated sprite example](https://calamity.sl4shed.xyz/example-animated-sprite)!
 */
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

    bool flipH = false;
    bool flipV = false;

    const Texture* getCurrentTexture() const;
    Vector2 getCurrentSize() const;
    bool isPlaying() const;
    Frame *getCurrentFrame() const;
    
    std::string getCurrentAnimationName() const;
    Animation *getCurrentAnimation() const;

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

/**
 * # Tilemap
 * Using the Tilemap class, you can draw \ref Tile "Tiles" to the screen. Tiles define a sourceRect, among other things.
 * 
 * Here is a basic example:
 * 
 * ```cpp
 * std::shared_ptr<Node> node = std::make_shared<Node>("tileNode");
 * // The tilesize Vector2 in the constructor represents the tile size that is rendered to the screen!
 * std::shared_ptr<Tilemap> map = std::make_shared<Tilemap>("res://path/to/texture.png", TextureScaling::PIXELART, Vector2{64.0f, 64.0f});
 * 
 * tilemap->addTiles(
 *       Tile(Vector2{-5, 0}, Rect{{0, 0}, {16, 16}}),
 *       Tile(Vector2{-4, 0}, Rect{{0, 0}, {16, 16}}),
 *       Tile(Vector2{-3, 0}, Rect{{0, 0}, {16, 16}}),
 *       Tile(Vector2{-2, 0}, Rect{{0, 0}, {16, 16}}),
 *       Tile(Vector2{-1, 0}, Rect{{0, 0}, {16, 16}}),
 *       Tile(Vector2{0, 0}, Rect{{0, 0}, {16, 16}}),
 * );
 * 
 * node->addComponent(map);
 * ```
 * 
 * Make sure to check out the [platformer example](https://calamity.sl4shed.xyz/example-platformer) too! It uses the Tilemap component for its map rendering.
 */
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
 * A base class for all scripts attached to nodes. It exposes the \ref Component "Components" lifetime functions, which the user can override and change.
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
 * And do this to add your script to a node:
 * ```cpp
 * // You must include the script before hand:
 * #include "scripts/ExampleScript.hpp"
 * 
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
 * 
 * The camera component it a virtual camera that defines the positions and scale/rotation of everything rendered on screen. Just like how cameras work in real life and in every other game engine!
 * 
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> node = std::make_shared<Node>("cameraNode");
 * std::shared_ptr<Camera> camera = std::make_shared<Camera>();
 * 
 * node->addComponent(camera);
 * ```
 * 
 * ## Properties and usages
 * The Camera component has an origin attribute, which allows you to render the camera offset depending on the position of its parent node:
 * ```cpp
 * camera->origin = {0.0f, 0.0f}; // The camera's top left position will now be the position of the node, as opposed to the default origin of {0.5f, 0.5f} which means the middle of the camera will be the position of the node.
 * ```
 * 
 * The Camera component also exposes a smoothing attribute, which allows you to smooth the cameras position.
 * ```cpp
 * camera->smoothing = 0.3f; // Now, whenever you move the camera, instead of moving linearly the camera will smoothly move over to the target position.
 * ```
 */
class Camera : public Component
{
public:
    void setActive();
    bool active = true;
    Vector2 origin = {0.5f, 0.5f};
    float smoothing = 0.0f;

    void update(float deltaTime) override;
    void initialize() override;

    Vector2 getGlobalPosition();
    Transform getCameraTransform() const;
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
private:
    Vector2 globalPos = Vector2{0.0f, 0.0f};
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