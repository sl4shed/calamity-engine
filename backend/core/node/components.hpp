#pragma once
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../services/input/keycode.hpp"
#include "../../services/input/input.hpp"

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
    virtual void update() {};
    virtual void initialize() {};
    virtual void input(InputEvent& event) {};
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
 * You add this to nodes that you want to have textures, pretty simple if you ask me.
 * Example usage:
 * ```
 * std::shared_ptr<Node> spriteNode = std::make_shared<Node>();
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
 * sprite->texture = Texture("path/to/texture.png");
 * spriteNode->addComponent(sprite);
 * ```
 */
class Sprite : public Component
{
public:
    Vector2 origin = {0.5f, 0.5f};
    Texture texture;
    Transform sourceTransform;
    bool visible = true;
    int zIndex = 1;

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
 * # Script component
 * A base class for all scripts attached to nodes to inherit from. It provides the necessary api's like update and start.
 *
 * To create a script and attach it to a node, create a header file somewhere like `scripts/ExampleScripy.hpp`:
 * ```cpp
 *
 * #pragma once
 *
 * #include "../backend/definitions.hpp"
 * #include <cereal/types/polymorphic.hpp>
 * #include <cereal/archives/json.hpp>
 * // and any other includes here based on where you put this file
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
 *     void start()
 *     {
 *         // start logic here
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
 * ExampleScript *exampleScript = new ExampleScript();
 * exampleNode->addComponent(exampleScript);
 * ```
 */
class Script : public Component
{
public:
    virtual void update(float deltaTime) {};
    virtual void start() {};
    virtual void physicsUpdate() {}; // todo

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
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Sprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Script)