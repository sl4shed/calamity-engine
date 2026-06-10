#pragma once
#include "definitions.hpp"
#include "../../core/definitions.hpp"
#include "../../core/node/components.hpp"

#include <box2d/base.h>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include <box2d/id.h>
#include <box2d/types.h>

#include <functional>

template <>
struct std::hash<b2ShapeId>
{
    std::size_t operator()(const b2ShapeId &id) const noexcept
    {
        // b2ShapeId has index1, world0, and revision fields
        std::size_t h = std::hash<uint32_t>{}(id.index1);
        h ^= std::hash<uint16_t>{}(id.world0) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<uint16_t>{}(id.generation) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

inline bool operator==(const b2ShapeId &a, const b2ShapeId &b) noexcept
{
    return a.index1 == b.index1 && a.world0 == b.world0 && a.generation == b.generation;
}

/**
 * # PhysicsBody
 * The base PhysicsBody class defines the Shape, functions and signals that are the base of every physics body.
 *
 * # Properties and usages
 * You can connect callbacks onto the collision signals of the physics body:
 * ```cpp
 * physicsBody->collisionEnter.connect([](PhysicsBody *other) {
 *    // do something when this body collides with another body
 * });
 *
 * // there are also colisionExit and collisionHit (when the body is hit by another body at terminal velocity).
 * ```
 *
 * There are also signals for when the mouse enters and leaves the body, which are very useful for UI elements.
 * ```cpp
 * physicsBody->mouseEntered.connect([]() {
 *  // do something when the mouse enters the body
 * });
 *
 * // there is also mouseExited for when the mouse leaves the body.
 * ```
 *
 * For more information, take a look at the [button example](https://calamity.sl4shed.xyz/example-button).
 */
class PhysicsBody : public Component
{
public:
    void exit() override;
    virtual void physicsUpdate() = 0;
    void initialize() override;
    virtual void initCompute();

    Signal<PhysicsBody *> collisionEnter;
    Signal<PhysicsBody *> collisionExit;
    Signal<PhysicsBody *> collisionHit;
    Signal<> mouseEntered;
    Signal<> mouseExited;
    std::shared_ptr<Shape> shape;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform), CEREAL_NVP(sensor));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(shape), CEREAL_NVP(storedTransform), CEREAL_NVP(sensor));
        initCompute();

        const float angle = storedTransform.getAngleRadians();
        b2Body_SetTransform(bodyId, storedTransform.position, {cos(angle), sin(angle)});
    }

    b2BodyId getBodyId() { return bodyId; };
    b2ShapeId getShapeId() { return shapeId; };

protected:
    bool sensor;
    b2BodyDef bodyDef;
    b2BodyId bodyId;
    b2ShapeId shapeId;

    Transform storedTransform;
};

/**
 * # Physics Service handles collisions and mouse collision logic. You can define the worlds gravity in its constructor:
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

    friend bool b2callback(b2ShapeId shapeId, void *ctx);
    void physicsUpdate(float timeStep);
    int subSteps = 4;

    void registerBody(PhysicsBody *body);
    void deRegisterBody(PhysicsBody *body);

    Vector2 gravity = {0.0f, 9.81f};
    b2WorldDef worldDef;
    b2WorldId worldId;

    PhysicsBody* findBodyFromShape(b2ShapeId id);
private:
    std::unordered_map<b2ShapeId, PhysicsBody *> bodyMap;

    std::vector<b2ShapeId> currentHovered;
    std::vector<b2ShapeId> lastHovered;
};

/**
 * # StaticBody
 *
 * The StaticBody component is a type of PhysicsBody. The main difference between StaticBody and any other physics bodies is that it does not move, at all, unless it is moved by the user.
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
class StaticBody : public PhysicsBody
{
public:
    StaticBody();
    StaticBody(const std::shared_ptr<Shape> &shape);

    void physicsUpdate() override;
    void initCompute() override;

    void setPosition(Vector2 pos) const;
    void setAngle(float angle) const;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<PhysicsBody>(this));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<PhysicsBody>(this));
    }
};

/**
 * # RigidBody
 *
 * The RigidBody is a type of PhysicsBody. The main difference between RigidBody and any other physics bodies is that it moves according to the physics simulation, and can be affected by forces, impulses, and collisions.
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
class RigidBody : public PhysicsBody
{
public:
    RigidBody();
    explicit RigidBody(const std::shared_ptr<Shape> &shape);

    void physicsUpdate() override;
    void initCompute() override;

    void setPosition(Vector2 pos) const;
    void setAngle(float angle) const;

    void setLinearVelocity(Vector2 vel) const;
    void applyForce(Vector2 force) const;
    void applyImpulse(Vector2 impulse) const;

    bool isOnGround();
    void lockRotation(bool value);
    Vector2 getLinearVelocity() const;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<PhysicsBody>(this), CEREAL_NVP(rotationLocked));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<PhysicsBody>(this), CEREAL_NVP(rotationLocked));
    }

private:
    bool rotationLocked = false;
};

/**
 * # KinematicBody
 *
 * The KinematicBody component is a type of PhysicsBody. The main difference between KinematicBody and any other physics body is that it does not move, at all, unless it is moved by a force or impulse or its linear velocity.
 *
 * Like any other physics body, once it is attached to a node, you must not move the node through its transform property.
 *
 * Example usage:
 * ```cpp
 * std::shared_ptr<Node> myNode = std::make_shared<Node>();
 * std::shared_ptr<Shape> myShape = std::make_shared<CircleShape>(20.0f); // Create a circle with a 20 pixel radius
 *
 * std::shared_ptr<KinematicBody> kinematicBody = std::make_shared<KinematicBody>(myShape);
 * myNode->addComponent(kinematicBody);
 * myNode->addComponent(std::make_shared<ShapeSprite>(myShape)); // also add a ShapeSprite for rendering!
 *
 * // later on, after the node is initialized, you can move the kinematic body by setting the linear velocity or applying impulses/forces:
 * kinematicBody->setLinearVelocity(Vector2{100.0f, 100.0f});
 * ```
 *
 * For more insight regarding anything physics related, the [physics example](https://calamity.sl4shed.xyz/example-physics) and [node tree example](https://calamity.sl4shed.xyz/example-node-tree) also utilize staticbodies!
 */
class KinematicBody : public PhysicsBody
{
public:
    KinematicBody();
    explicit KinematicBody(const std::shared_ptr<Shape> &shape);

    void physicsUpdate() override;
    void initCompute() override;

    void setLinearVelocity(Vector2 vel) const;
    void applyForce(Vector2 force) const;
    void applyImpulse(Vector2 impulse) const;

    Vector2 getLinearVelocity() const;

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<PhysicsBody>(this));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<PhysicsBody>(this));
    }
};

CEREAL_REGISTER_TYPE(RigidBody);
CEREAL_REGISTER_TYPE(StaticBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, RigidBody);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, StaticBody);