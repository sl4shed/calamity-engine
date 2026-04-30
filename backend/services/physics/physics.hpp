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
 * The physics service is a wrapper around Box2D. You can define the worlds gravity in its constructor:
 * 
 * ```cpp
 * Physics physics = Physics(Vector2{0.0f, 9.81f});
 * ```
 */
class Physics
{
public:
    Physics(Vector2 gravity = {0.0f, 9.81f});
    void exit();

    void physicsUpdate(float timeStep);
    int subSteps = 4;

    Vector2 gravity = {0.0f, 9.81f};
    b2WorldDef worldDef;
    b2WorldId worldId;
};

/**
 * # StaticBody
 * 
 * The StaticBody components allows a Node to collide with other physics bodies. The main difference between StaticBody and any other physics body is that it does not move, at all, unless it is moved by the user.
 * 
 * Like any other physics body, once it is attached to a node, you must not move the node through its transform property but through the `setPosition()` and `setAngle()` functions of the physics body.
 * 
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> myNode = std::make_shared<Node>();
 * std::shared_ptr<Shape> myShape = std::make_shared<CircleShape>(20.0f); // Create a circle with a 20 pixel radius
 * 
 * std::shared_ptr<StaticBody> staticBody = std::make_shared<StaticBody>(myShape);
 * myNode->addComponent(staticBody);
 * myNode->addComponent(std::make_shared<ShapeSprite>(myShape)); // also add a ShapeSprite for rendering!
 * 
 * // later on, after the node is initialized, you can move the static body:
 * staticBody->setPosition(Vector2{100.0f, 100.0f});
 * ```
 * 
 * For more insight regarding anything physics related, the [physics example](https://calamity.sl4shed.xyz/example-physics) and [node tree example](https://calamity.sl4shed.xyz/example-node-tree) also utilize staticbodies!
 */
class StaticBody : public Component
{
public:
    StaticBody();
    StaticBody(const std::shared_ptr<Shape>& shape);

    void exit() override;
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

/**
 * # RigidBody
 * 
 * The RigidBody components allows a Node to collide with other physics bodies and to interact with physics forces and stuff. 
 * 
 * Like any other physics body, once it is attached to a node, you must not move the node through its transform property but through the `setPosition()` and `setAngle()` functions of the physics body.
 * 
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> myNode = std::make_shared<Node>();
 * std::shared_ptr<Shape> myShape = std::make_shared<CircleShape>(20.0f); // Create a circle with a 20 pixel radius
 * 
 * std::shared_ptr<RigidBody> rigidBody = std::make_shared<RigidBody>(myShape);
 * myNode->addComponent(rigidBody);
 * myNode->addComponent(std::make_shared<ShapeSprite>(myShape)); // also add a ShapeSprite for rendering!
 * 
 * // later on, after the node is initialized, you can move the rigid body:
 * rigidBody->setPosition(Vector2{100.0f, 100.0f});
 * ```
 * 
 * ## Functions and usages
 * You can apply forces and impulses:
 * ```cpp
 * rigidBody->applyforce({1.0f, 0.0f});
 * rigidBody->applyImpulse({1.0f, 0.0f});
 * ```
 * 
 * You can also set its linear velocity. Instead of setting its position every frame, if you have something like a player controller you should instead use `setLinearVelocity()`:
 * ```cpp
 * Vector2 velocity = rigidBody->getLinearVelocity();
 * rigidBody->setLinearVelocity(Vector2{moveDirection, velocity.y});
 * ```
 * 
 * You can check if a rigidBody is on the ground by using the `isOnGround()` function:
 * ```cpp
 * if(rigidBody->isOnGround() && Input.isActionJustPressed("jump")) {
 *      Vector2 velocity = rigidBody->getLinearVelocity();
 *      rigidBody->setLinearVelocity(Vector2{velocity.x, velocity.y - 5});   
 * }
 * ```
 * > For more information regarding a player controller, check out the [platformer example](https://calamity.sl4shed.xyz/example-platformer)!
 * 
 * You can also lock its rotation:
 * ```cpp
 * rigidBody->lockRotation(true);
 * ```
 * This would be used in something like a top-down character controller, where you dont want your player rotating during collisions and other stuff.
 * For more information regarding a top-down player controller, check out the [top down example](https://calamity.sl4shed.xyz/example-top-down)!
 * 
 * For more insight regarding anything physics related, the [physics example](https://calamity.sl4shed.xyz/example-physics) and [node tree example](https://calamity.sl4shed.xyz/example-node-tree) also utilize rigidbodies!
 */
class RigidBody : public Component
{
public:
    RigidBody();
    explicit RigidBody(const std::shared_ptr<Shape>& shape);

    void exit() override;
    void physicsUpdate() override;
    void initialize() override;
    void initCompute();

    void setPosition(Vector2 pos) const;
    void setAngle(float angle) const;

    void setLinearVelocity(Vector2 vel) const;
    void applyForce(Vector2 force) const;
    void applyImpulse(Vector2 impulse) const;

    bool isOnGround();
    void lockRotation(bool value);
    Vector2 getLinearVelocity() const;

    std::shared_ptr<Shape> shape;
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform), CEREAL_NVP(rotationLocked));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform), CEREAL_NVP(rotationLocked));
        initCompute();

        const float angle = storedTransform.getAngleRadians();
        b2Body_SetTransform(bodyId, storedTransform.position, {cos(angle), sin(angle)});
    }

private:
    b2BodyDef bodyDef;
    b2BodyId bodyId;

    Transform storedTransform;
    bool rotationLocked = false;
};

/**
 * # KinematicBody
 * 
 * The KinematicBody components allows a Node to collide with other physics bodies. The main difference between KinematicBody and any other physics body is that it does not move, at all, unless it is moved by a force or impulse or its linear velocity.
 * 
 * Like any other physics body, once it is attached to a node, you must not move the node through its transform property.
 * 
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> myNode = std::make_shared<Node>();
 * std::shared_ptr<Shape> myShape = std::make_shared<CircleShape>(20.0f); // Create a circle with a 20 pixel radius
 * 
 * std::shared_ptr<StaticBody> kinematicBody = std::make_shared<KinematicBody>(myShape);
 * myNode->addComponent(kinematicBody);
 * myNode->addComponent(std::make_shared<ShapeSprite>(myShape)); // also add a ShapeSprite for rendering!
 * 
 * // later on, after the node is initialized, you can move the kinematic body by setting the linear velocity or applying impulses/forces:
 * kinematicBody->setLinearVelocity(Vector2{100.0f, 100.0f});
 * ```
 * 
 * For more insight regarding anything physics related, the [physics example](https://calamity.sl4shed.xyz/example-physics) and [node tree example](https://calamity.sl4shed.xyz/example-node-tree) also utilize staticbodies!
 */
class KinematicBody : public Component
{
public:
    KinematicBody();
    explicit KinematicBody(const std::shared_ptr<Shape>& shape);

    void exit() override;
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