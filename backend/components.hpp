#pragma once
#include "definitions.hpp"
class Engine; // Forward declaration to avoid circular include
class Node; // Forward declaration

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
    Texture texture;
    Transform source_transform;
    bool visible;
    int z_index;
};

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
};