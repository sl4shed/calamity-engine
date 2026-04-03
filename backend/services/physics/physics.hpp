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
    b2WorldDef worldDef;
    b2WorldId worldId;
};

struct Polygon {
Polygon();
explicit Polygon(const b2Polygon &polygon);

Vector2 centroid;
int count;
Vector2 normals[B2_MAX_POLYGON_VERTICES];
float radius;
Vector2 vertices[B2_MAX_POLYGON_VERTICES];

operator b2Polygon() const;
static Polygon Box(Vector2 size, Vector2 center = {0.5f, 0.5f}, float angle = 0.0f);
};


class Shape {
public:
    Polygon polygon;
    b2ShapeDef shapeDef;
};

class BoxShape : public Shape {
public:
    BoxShape(Vector2 size, Vector2 center = {0.5f, 0.5f});
    Vector2 size;
    Vector2 center;
};

class StaticBody : public Component {
public:
    StaticBody();
    StaticBody(std::shared_ptr<Shape> shape);

    std::shared_ptr<Shape> shape;
private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;
};

class RigidBody : public Component {
public:
    RigidBody(std::shared_ptr<Shape> shape);

    std::shared_ptr<Shape> shape;
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