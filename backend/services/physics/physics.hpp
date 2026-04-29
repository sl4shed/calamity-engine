#pragma once
#include "definitions.hpp"
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

    Vector2 gravity = {0.0f, 9.81f};
    b2WorldDef worldDef;
    b2WorldId worldId;
};

class StaticBody : public Component
{
public:
    StaticBody();
    ~StaticBody() override;
    StaticBody(const std::shared_ptr<Shape>& shape);

    void physicsUpdate() override;
    void initialize() override;
    void initCompute();

    void setPosition(Vector2 pos) const;
    void setAngle(float angle) const;

    std::shared_ptr<Shape> shape;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
        initCompute();

        const float angle = storedTransform.getAngleRadians();
        b2Body_SetTransform(bodyId, storedTransform.position, {cos(angle), sin(angle)});
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Transform storedTransform;
};

class RigidBody : public Component
{
public:
    RigidBody();
    ~RigidBody() override;
    explicit RigidBody(const std::shared_ptr<Shape>& shape);

    void physicsUpdate() override;
    void initialize() override;
    void initCompute();

    void fixRotation(bool value) const;

    void setPosition(Vector2 pos) const;
    void setAngle(float angle) const;

    void setLinearVelocity(Vector2 vel) const;
    void applyForce(Vector2 force) const;
    void applyImpulse(Vector2 impulse) const;

    Vector2 getLinearVelocity() const;

    std::shared_ptr<Shape> shape;
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
        initCompute();

        const float angle = storedTransform.getAngleRadians();
        b2Body_SetTransform(bodyId, storedTransform.position, {cos(angle), sin(angle)});
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Transform storedTransform;
};

class KinematicBody : public Component
{
public:
    KinematicBody();
    ~KinematicBody() override;
    explicit KinematicBody(const std::shared_ptr<Shape>& shape);

    void physicsUpdate() override;
    void initialize() override;
    void initCompute();

    void setLinearVelocity(Vector2 vel) const;
    void applyForce(Vector2 force) const;
    void applyImpulse(Vector2 impulse) const;

    Vector2 getLinearVelocity() const;

    std::shared_ptr<Shape> shape;
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform));
        initCompute();

        const float angle = storedTransform.getAngleRadians();
        b2Body_SetTransform(bodyId, storedTransform.position, {cos(angle), sin(angle)});
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Transform storedTransform;
};


CEREAL_REGISTER_TYPE(RigidBody);
CEREAL_REGISTER_TYPE(StaticBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, RigidBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, StaticBody);