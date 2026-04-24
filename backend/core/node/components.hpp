#pragma once
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../services/input/keycode.hpp"
#include "../../services/input/input.hpp"
#include "../ui/definitions.hpp"

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

    Node *getNode();
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
    Sprite(std::string texturePath);

    Vector2 origin = {0.5f, 0.5f};
    Texture texture;
    Transform sourceTransform;
    bool visible = true;
    int zIndex = 1;
    bool screenSpace = false;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceTransform), CEREAL_NVP(zIndex), CEREAL_NVP(visible), CEREAL_NVP(texture));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(sourceTransform), CEREAL_NVP(zIndex), CEREAL_NVP(visible), CEREAL_NVP(texture));
    }
};

/**
 * # ShapeSprite
 * A dynamic sprite (mainly used for debugging) which can draw solid color filled shapes.
 *
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> spriteNode = std::make_shared<Node>();
 * Polygon shape = Polygon();
 * // populate polygon here, do whatever you want
 * std::shared_ptr<ShapeSprite> sprite = std::make_shared<ShapeSprite>(shape);
 * spriteNode->addComponent(sprite);
 * ```
 */
class ShapeSprite : public Component
{
public:
    ShapeSprite();
    ShapeSprite(Polygon shape);

    Vector2 origin = {0.5f, 0.5f};
    Polygon shape;
    bool visible = true;
    int zIndex = 1;
    bool screenSpace = false;
    Color color = Color::WHITE;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(shape), CEREAL_NVP(zIndex), CEREAL_NVP(visible));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(origin), CEREAL_NVP(shape), CEREAL_NVP(zIndex), CEREAL_NVP(visible));
    }
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

    void initialize();

    Vector2 screenToWorld(Vector2 screen);

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
CEREAL_REGISTER_TYPE(ShapeSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Sprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ShapeSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Script)