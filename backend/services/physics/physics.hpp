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

// TODO: make this actually work with shapeDefs and shit
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
    virtual ~Shape() = default;
    Polygon scaledPolygon;
    Polygon polygon;
    b2ShapeDef shapeDef;
    Vector2 origin;

    void applyMaterial(Material material);

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
        ar(CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon), CEREAL_NVP(material), CEREAL_NVP(origin));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(scaledPolygon), CEREAL_NVP(polygon), CEREAL_NVP(material), CEREAL_NVP(origin));
    }
private:
    Material material;
};

class BoxShape : public Shape
{
public:
    BoxShape() = default;
    BoxShape(Vector2 size, Vector2 origin = {0.5f, 0.5f});
    Vector2 size;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(size));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Shape>(this), CEREAL_NVP(size));

        // just reconstruct everything from size and center like the constructor does
        Vector2 calculatedCenter = origin * size;
        b2Rot rotation = {cos(0.0f), sin(0.0f)};

        b2Polygon poly = b2MakeOffsetBox(size.x / 2 * Physics::scale, size.y / 2 * Physics::scale, (b2Vec2)(calculatedCenter * Physics::scale), rotation);
        b2Polygon polyUnscaled = b2MakeOffsetBox(size.x / 2, size.y / 2, (b2Vec2)calculatedCenter, rotation);

        this->polygon = (Polygon)polyUnscaled;
        this->scaledPolygon = (Polygon)poly;
        this->shapeDef = b2DefaultShapeDef();
    }
};

class RoundedBoxShape : public Shape
{
public:
    RoundedBoxShape(Vector2 size, Vector2 origin = {0.5f, 0.5f}, float radius = 0.1f);
    Vector2 size;
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

    void setPosition(Vector2 pos);
    void setAngle(float angle);

    std::shared_ptr<Shape> shape;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape));

        bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2BodyType::b2_staticBody;
        bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
        b2Polygon poly = shape->scaledPolygon;
        b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;
};

class RigidBody : public Component
{
public:
    RigidBody();
    ~RigidBody();
    RigidBody(std::shared_ptr<Shape> shape);

    void physicsUpdate();
    void initialize();

    void setPosition(Vector2 pos);
    void setAngle(float angle);

    void setLinearVelocity(Vector2 vel);
    void applyForce(Vector2 force);
    void applyImpulse(Vector2 impulse);

    std::shared_ptr<Shape> shape;
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape));

        bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2BodyType::b2_dynamicBody;
        bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
        b2Polygon poly = shape->scaledPolygon;
        b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;
};


CEREAL_REGISTER_TYPE(RigidBody);
CEREAL_REGISTER_TYPE(StaticBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, RigidBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, StaticBody);

CEREAL_REGISTER_TYPE(BoxShape);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Shape, BoxShape);