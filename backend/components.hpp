#ifndef HPP_COMPONENTS
#define HPP_COMPONENTS

#include "definitions.hpp"

class Node; // Forward declaration

struct Component
{
    virtual ~Component() = default;
    void update() {};
    Node *getNode();
    void setNode(Node *node);

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
    void update() {};
    void start() {};
    void physicsUpdate() {}; // todo
};

#endif