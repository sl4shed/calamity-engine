#include "physics.hpp"
#include "../services.hpp"
#include "../../core/node/node.hpp"
#include "../../utils/logger.hpp"

void Shape::applyMaterial(const Material& mat)
{
    this->material = mat;
    shapeDef.density = mat.density;
    shapeDef.material.friction = mat.friction;
    shapeDef.material.restitution = mat.restitution;
    shapeDef.material.rollingResistance = mat.rollingResistance;
    shapeDef.material.tangentSpeed = mat.tangentSpeed;
}

Physics::Physics(const Vector2 gravity) {
    this->worldDef = b2DefaultWorldDef();
    this->worldDef.gravity = gravity;
    this->worldId = b2CreateWorld(&worldDef);
}

void Physics::physicsUpdate(const float timeStep) {
    b2World_Step(worldId, timeStep, subSteps);
}

Physics::~Physics() {
    b2DestroyWorld(worldId);
}

////////////////////
// rigid body //////
///////////////////

void RigidBody::initCompute()
{
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 0.1f;
    bodyDef.angularDamping = 0.1f;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);

    if (const auto* box = dynamic_cast<BoxShape*>(shape.get()))
    {
        const b2Polygon poly = box->scaledPolygon;
        b2CreatePolygonShape(bodyId, &box->shapeDef, &poly);
    }
    else if (const auto* circle = dynamic_cast<CircleShape*>(shape.get()))
    {
        const b2Circle c = circle->scaledCircle;
        b2CreateCircleShape(bodyId, &circle->shapeDef, &c);
    }
    else if (const auto* capsule = dynamic_cast<CapsuleShape*>(shape.get()))
    {
        const b2Capsule c = capsule->scaledCapsule;
        b2CreateCapsuleShape(bodyId, &capsule->shapeDef, &c);
    }
}

RigidBody::RigidBody() {}
RigidBody::RigidBody(const std::shared_ptr<Shape>& shape) {
    this->shape = shape;
    initCompute();
}

void RigidBody::physicsUpdate() {
    Node* node = getNode();

    // send box2d pos to world pos
    const Vector2 worldPos = Vector2(b2Body_GetPosition(bodyId)) / PhysicsConstants::scale;
    const float worldAngle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));

    if (node->parent) {
        auto [position, transformation] = node->parent->globalTransform.inverse();
        node->transform.position = transformation * (worldPos - node->parent->globalTransform.position);
        node->transform.setAngleRadians(worldAngle - node->parent->globalTransform.getAngle());
    } else {
        node->transform.position = worldPos;
        node->transform.setAngleRadians(worldAngle);
    }

    storedTransform.position = Vector2(b2Body_GetPosition(bodyId));
    storedTransform.setAngleRadians(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));
}

void RigidBody::setPosition(const Vector2 pos) const
{
    b2Body_SetTransform(bodyId, pos, b2Body_GetRotation(bodyId));
    // this is supposed to null velocity but i think it does that anyway?
}

void RigidBody::setAngle(const float angle) const
{
    b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(angle), sin(angle)});
    // this is supposed to null velocity but idk
}

void RigidBody::setLinearVelocity(const Vector2 vel) const
{
    b2Body_SetLinearVelocity(bodyId, vel);
}

void RigidBody::applyForce(const Vector2 force) const
{
    b2Body_ApplyForce(bodyId, force, shape->origin, true);
    // i guess, always wake the body? that makes the most sense
}

void RigidBody::applyImpulse(const Vector2 impulse) const
{
    b2Body_ApplyLinearImpulse(bodyId, impulse, shape->origin, true);
}

void RigidBody::initialize() {
    const auto tr = this->getNode()->globalTransform;
    b2Body_SetTransform(bodyId, tr.position * PhysicsConstants::scale, {cos(tr.getAngleRadians()), sin(tr.getAngleRadians())});
}

void RigidBody::fixRotation(const bool value) const
{
    b2Body_SetFixedRotation(bodyId, value);
}

RigidBody::~RigidBody() {
    b2DestroyBody(bodyId);
}

/////////////////////////////
// static body             //
/////////////////////////////

void StaticBody::initCompute()
{
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.linearDamping = 0.1f;
    bodyDef.angularDamping = 0.1f;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);

    if (const auto* box = dynamic_cast<BoxShape*>(shape.get()))
    {
        const b2Polygon poly = box->scaledPolygon;
        b2CreatePolygonShape(bodyId, &box->shapeDef, &poly);
    }
    else if (const auto* circle = dynamic_cast<CircleShape*>(shape.get()))
    {
        const b2Circle c = circle->scaledCircle;
        b2CreateCircleShape(bodyId, &circle->shapeDef, &c);
    }
    else if (const auto* capsule = dynamic_cast<CapsuleShape*>(shape.get()))
    {
        const b2Capsule c = capsule->scaledCapsule;
        b2CreateCapsuleShape(bodyId, &capsule->shapeDef, &c);
    }
}

StaticBody::StaticBody() {}
StaticBody::StaticBody(const std::shared_ptr<Shape>& shape) {
    this->shape = shape;
    initCompute();
}

void StaticBody::physicsUpdate() {
    storedTransform.position = Vector2(b2Body_GetPosition(bodyId));
    storedTransform.setAngleRadians(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));
}

void StaticBody::setPosition(const Vector2 pos) const
{
    b2Body_SetTransform(bodyId, (b2Vec2)pos, b2Body_GetRotation(bodyId));
    // this is supposed to null velocity but i think it does that anyway?
}

void StaticBody::setAngle(const float angle) const
{
    b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(angle), sin(angle)});
    // this is supposed to null velocity but idk
}


void StaticBody::initialize() {
    const auto tr = this->getNode()->globalTransform;
    b2Body_SetTransform(bodyId, tr.position * PhysicsConstants::scale, {cos(tr.getAngleRadians()), sin(tr.getAngleRadians())});
}

StaticBody::~StaticBody() {
    b2DestroyBody(bodyId);
}

////////////////////
// kinematic body //
///////////////////

void KinematicBody::initCompute()
{
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_kinematicBody;
    bodyDef.linearDamping = 0.1f;
    bodyDef.angularDamping = 0.1f;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);

    if (const auto* box = dynamic_cast<BoxShape*>(shape.get()))
    {
        const b2Polygon poly = box->scaledPolygon;
        b2CreatePolygonShape(bodyId, &box->shapeDef, &poly);
    }
    else if (const auto* circle = dynamic_cast<CircleShape*>(shape.get()))
    {
        const b2Circle c = circle->scaledCircle;
        b2CreateCircleShape(bodyId, &circle->shapeDef, &c);
    }
    else if (const auto* capsule = dynamic_cast<CapsuleShape*>(shape.get()))
    {
        const b2Capsule c = capsule->scaledCapsule;
        b2CreateCapsuleShape(bodyId, &capsule->shapeDef, &c);
    }
}

KinematicBody::KinematicBody() {}
KinematicBody::KinematicBody(const std::shared_ptr<Shape>& shape) {
    this->shape = shape;
    initCompute();
}

void KinematicBody::physicsUpdate() {
    Node* node = getNode();

    // send box2d pos to world pos
    const Vector2 worldPos = Vector2(b2Body_GetPosition(bodyId)) / PhysicsConstants::scale;
    const float worldAngle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));

    if (node->parent) {
        auto [position, transformation] = node->parent->globalTransform.inverse();
        node->transform.position = transformation * (worldPos - node->parent->globalTransform.position);
        node->transform.setAngleRadians(worldAngle - node->parent->globalTransform.getAngle());
    } else {
        node->transform.position = worldPos;
        node->transform.setAngleRadians(worldAngle);
    }

    storedTransform.position = Vector2(b2Body_GetPosition(bodyId));
    storedTransform.setAngleRadians(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));
}

void KinematicBody::setLinearVelocity(const Vector2 vel) const
{
    b2Body_SetLinearVelocity(bodyId, vel);
}

void KinematicBody::applyForce(const Vector2 force) const
{
    b2Body_ApplyForce(bodyId, force, shape->origin, true);
    // i guess, always wake the body? that makes the most sense
}

void KinematicBody::applyImpulse(const Vector2 impulse) const
{
    b2Body_ApplyLinearImpulse(bodyId, impulse, shape->origin, true);
}

// this is the one exception or something?
// i have zero idea if this will work :uuh:
void KinematicBody::initialize() {
    const auto tr = this->getNode()->globalTransform;
    b2Body_SetTransform(bodyId, tr.position * PhysicsConstants::scale, {cos(tr.getAngleRadians()), sin(tr.getAngleRadians())});
}

KinematicBody::~KinematicBody() {
    b2DestroyBody(bodyId);
}