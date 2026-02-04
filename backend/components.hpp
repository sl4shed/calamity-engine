#pragma once
#include "definitions.hpp"
class Engine; // Forward declaration to avoid circular include
class Node;   // Forward declaration

struct Component
{
    virtual ~Component() = default;
    virtual void update() {};
    Node *getNode();
    void setNode(Node *n);

private:
    Node *node;
};

class Sprite : public Component
{
public:
    Vector2 origin;
    Texture texture;
    Transform sourceTransform;
    bool visible;
    int zIndex;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(origin, texture, sourceTransform, visible, zIndex, node);
    }
};

/**
 * A base class for all scripts attached to nodes to inherit from. It provides the necessary api's like update and start.
 *
 * To create a script and attach it to a node, create a header file somewhere like `scripts/ExampleScripy.hpp`:
 * ```cpp
 *
 * #pragma once
 *
 * #include "../backend/definitions.hpp"
 * // and any other includes here based on where you put this file
 *
 * class ExampleScript : public Script
 * {
 *
 * public:
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
 * ```
 *
 * and do this to your node:
 * ```cpp
 * Node *exampleNode = new Node();
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
};

class Camera : public Component
{
public:
    void setActive(Engine &engine);
    bool active;
    Vector2 origin = {0.5f, 0.5f};

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(active, origin);
    }
};