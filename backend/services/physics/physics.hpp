#pragma once

#include "../../core/definitions.hpp"
#include "../../core/node/node.hpp"
#include "../../core/node/components.hpp"

#include <box2d/base.h>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include <box2d/id.h>
#include <box2d/types.h>

class Physics {
public:
    Physics(Vector2 gravity = {0.0f, -9.81f});

    void update(float deltaTime);
    void initialize();

    Vector2 gravity = {0.0f, -9.81f};
private:
    b2WorldDef worldDef;
    b2WorldId worldId;
};

struct Polygon {
Vector2 centroid;
int count;
Vector2 normals[B2_MAX_POLYGON_VERTICES];
float radius;
Vector2 vertices[B2_MAX_POLYGON_VERTICES];


static Polygon Box();
};


class StaticBody : public Component {
public:
    StaticBody();
private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;
};

class RigidBody : public Component {
public:
    RigidBody();


    template <class Archive>
    void save(Archive &ar) const
    {
        
    }

    template <class Archive>
    void load(Archive &ar)
    {
        
    }
private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;
};