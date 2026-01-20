#pragma once
#include "definitions.hpp"

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

struct Transform : public Component
{
    Vector2D position;
    Vector2D scale;
    Vector2D origin;
    float angle;
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
    virtual void update() {};
    virtual void start() {};
    virtual void physicsUpdate() {}; // todo
};