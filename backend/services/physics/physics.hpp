#pragma once

#include "../../core/definitions.hpp"
#include "../../core/node/components.hpp"

#include <box2d/base.h>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include <box2d/id.h>
#include <box2d/types.h>

/**
 * # Physics Service
 *
 */
class Physics
{
public:
    Physics(Vector2 gravity = {0.0f, 9.81f});
    ~Physics();

    void physicsUpdate(float timeStep);
    int subSteps = 4;
    static float constexpr scale = 0.01f; // 100 pixels = 1 meter

    Vector2 gravity = {0.0f, 9.81f};
    b2WorldDef worldDef;
    b2WorldId worldId;
};

class Material
{
public:
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;
    float rollingResistance = 0.0f;
    float tangentSpeed = 0.0f;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(density), CEREAL_NVP(friction), CEREAL_NVP(restitution), CEREAL_NVP(rollingResistance), CEREAL_NVP(tangentSpeed));
    }
};

class Shape
{
public:
    Polygon scaledPolygon;
    Polygon polygon;
    b2ShapeDef shapeDef;
    Material material;

    Shape *setSensor(bool sensor)
    {
        shapeDef.isSensor = sensor;
        if (sensor)
        {
            shapeDef.enableSensorEvents = true;
        }
        return this;
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(scaledPolygon, polygon, material);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(scaledPolygon, polygon, material);
    }
};

class BoxShape : public Shape
{
public:
    BoxShape(Vector2 size, Vector2 center = {0.5f, 0.5f});
    Vector2 size;
    Vector2 center;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), size, center);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), size, center);
        this->shapeDef = b2DefaultShapeDef();
    }
};

class RoundedBoxShape : public Shape
{
public:
    RoundedBoxShape(Vector2 size, Vector2 center = {0.5f, 0.5f}, float radius = 0.1f);
    Vector2 size;
    Vector2 center;
    float radius;
};

class CircleShape : public Shape
{
};

class CapsuleShape : public Shape
{
};

class StaticBody : public Component
{
public:
    StaticBody();
    ~StaticBody();
    StaticBody(std::shared_ptr<Shape> shape);

    void physicsUpdate();
    void initialize();

    std::shared_ptr<Shape> shape;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(shape);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(shape);

        bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2BodyType::b2_staticBody;
        bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
        b2Polygon poly = shape->scaledPolygon;
        b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Vector2 prevPosition;
    float prevAngle;
};

class RigidBody : public Component
{
public:
    RigidBody();
    ~RigidBody();
    RigidBody(std::shared_ptr<Shape> shape);

    void physicsUpdate();
    void initialize();

    std::shared_ptr<Shape> shape;
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(shape);
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(shape);

        bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2BodyType::b2_dynamicBody;
        bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
        b2Polygon poly = shape->scaledPolygon;
        b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Vector2 prevPosition;
    float prevAngle;
};
